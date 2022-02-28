#include <llvm-supermutate/MutationEngine.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>


namespace llvmsupermutate {

MutationEngine::MutationEngine(
  llvm::Module &module,
  LLVMToSourceMapping *sourceMapping
) :
  module(module),
  context(module.getContext()),
  sourceMapping(sourceMapping),
  loaderFunction(nullptr),
  mutators(),
  mutations(),
  instructionToClone(),
  instructionToSwitchMap(),
  instructionToPhiMap(),
  instructionToDestinationBlock(),
  instructionToCloneBlock()
{
  prepare();
}

void MutationEngine::writeMutatedBitcode(std::string const &filename) {
  llvm::outs() << "writing mutated bitcode to file: " << filename << "\n";

  // TODO remove or else put this into a log file
  // DEBUGGING
  // module.dump();
  // llvm::outs() << "\n";

  std::error_code error_code;
  llvm::raw_fd_ostream out(filename, error_code);
  llvm::WriteBitcodeToFile(module, out);
  llvm::outs() << "wrote mutated bitcode to file: " << filename << "\n";
}

void MutationEngine::mutate(llvm::Instruction *instruction) {
  if (hasBeenMutated(instruction)) {
    llvm::outs()
      << "WARNING: ignore mutate request: instruction already mutated ["
      << std::addressof(instruction)
      << "]\n";
    return;
  }

  llvm::outs() << "DEBUG: mutating LLVM instruction [" << std::addressof(instruction) << "]: ";

  for (auto *mutator : mutators) {
    mutator->mutate(instruction);
  }

  // finally, now that we're done with it, we erase the original instruction
  // UNLESS no mutations were actually applied to that instruction
  if (hasBeenMutated(instruction)) {
    instruction->eraseFromParent();
  }
}

void MutationEngine::addMutator(InstructionMutator *mutator) {
  mutators.push_back(mutator);
}

void MutationEngine::inject(InstructionMutation *mutation) {
  auto *instruction = mutation->getOriginal();

  // if we haven't injected a mutation at this location yet, we should add the necessary
  // instrumentation to that location
  if (!hasBeenMutated(instruction)) {
    prepareInstruction(instruction);
  }

  llvm::IRBuilder<> builder(context);

  // assign an ID to the mutant and log it
  size_t id = nextMutantId++;
  mutations.push_back(mutation);

  // create a new block for this mutation and add it to the switch
  auto *mutantSwitch = instructionToSwitchMap[instruction];
  auto *function = mutantSwitch->getFunction();
  auto *mutantBlock = llvm::BasicBlock::Create(context, "", function);
  mutantSwitch->addCase(builder.getInt64(id), mutantBlock);

  // find the phi node for the original instruction
  llvm::PHINode *phi = nullptr;
  if (instructionToPhiMap.find(instruction) != instructionToPhiMap.end()) {
    phi = instructionToPhiMap[instruction];
  }

  // inject the mutated/replacement instruction into the generated block
  mutation->inject(
    mutantBlock,
    instructionToCloneBlock[instruction],
    instructionToDestinationBlock[instruction],
    phi
  );
}

bool MutationEngine::hasBeenMutated(llvm::Instruction *instruction) const {
  return instructionToSwitchMap.find(instruction) != instructionToSwitchMap.end();
}

void MutationEngine::prepare() {
  // https://stackoverflow.com/questions/22310091/how-to-declare-a-function-in-llvm-and-define-it-later
  auto *i8PtrType = llvm::Type::getInt8PtrTy(context);
  auto *i32Type = llvm::Type::getInt32Ty(context);
  auto *i64Type = llvm::Type::getInt64Ty(context);
  auto *i8PtrPtrType = llvm::PointerType::getUnqual(i8PtrType);

  // ; Function Attrs: nounwind
  // declare dso_local i8* @getenv(i8*) #2
  llvm::Function *getenvFunction = module.getFunction("getenv");
  if (getenvFunction == nullptr) {
    std::vector<llvm::Type*> getenvArgTypes = {i8PtrType};
    auto *getenvType = llvm::FunctionType::get(i8PtrType, getenvArgTypes, false);
    getenvFunction = llvm::Function::Create(
      getenvType,
      llvm::Function::ExternalLinkage,
      "getenv",
      module
    );
  }

  // ; Function Attrs: nounwind
  // declare dso_local i64 @strtol(i8*, i8**, i32) #2
  llvm::Function *strtolFunction = module.getFunction("strtol");
  if (strtolFunction == nullptr) {
    std::vector<llvm::Type*> strtolArgTypes = {i8PtrType, i8PtrPtrType, i32Type};
    auto *strtolType = llvm::FunctionType::get(i64Type, strtolArgTypes, false);
    strtolFunction = llvm::Function::Create(
      strtolType,
      llvm::Function::ExternalLinkage,
      "strtol",
      module
    );
  }

  // declare dso_local i32 @printf(i8* %0, ...) #3
  llvm::Function *printFunction = module.getFunction("printf");
  if (printFunction == nullptr) {
    auto *printType = llvm::FunctionType::get(i32Type, {i8PtrType}, true);
    printFunction = llvm::Function::Create(
      printType,
      llvm::Function::ExternalLinkage,
      "printf",
      module
    );
  }

  // create a new function for loading mutation switch values
  static std::string loaderFunctionName = "__llvm_mutate_load_mutations__";
  module.getOrInsertFunction(loaderFunctionName, llvm::Type::getVoidTy(context));
  loaderFunction = module.getFunction(loaderFunctionName);

  // add initial function definition
  auto *loaderFunctionEntryBlock = llvm::BasicBlock::Create(context, "entry", loaderFunction);
  llvm::ReturnInst::Create(context, loaderFunctionEntryBlock);

  // insert a call to the loader function at the start of the program
  // for now, this is fairly hardcoded to C/C++ by injecting code to the start of
  // the @main function; we can make this more universal later
  auto *mainFunction = module.getFunction("main");
  llvm::CallInst::Create(
    loaderFunction->getFunctionType(),
    loaderFunction,
    "",
    mainFunction->getEntryBlock().getFirstNonPHI()
  );
}

llvm::Value* MutationEngine::prepareInstructionLoader(llvm::Instruction *instruction) {
  llvm::outs() << "DEBUG: preparing mutation loader for instruction [";
  instruction->print(llvm::outs());
  llvm::outs() << "]\n";

  llvm::IRBuilder<> builder(context);

  // we essentially create a new entrypoint for the loader function
  auto *entryBlock = &loaderFunction->getEntryBlock();
  auto *successorBlock = entryBlock->splitBasicBlock(&entryBlock->front());
  builder.SetInsertPoint(&entryBlock->front());

  auto *i8Type = builder.getInt8Ty();
  auto *i8PtrType = builder.getInt8PtrTy();
  auto *i64Type = builder.getInt64Ty();
  auto *zeroConstant = builder.getInt64(0);

  // create a global variable to store the value of which mutant should be used for this instruction
  std::stringstream varnameBuffer;
  varnameBuffer
    << "__llvmrepair_instruction_mutant_"
    << sourceMapping->get(instruction)->getID()
    << "__";
  llvm::GlobalVariable *global = llvm::cast<llvm::GlobalVariable>(
      module.getOrInsertGlobal(varnameBuffer.str(), i64Type)
  );
  global->setInitializer(zeroConstant);

  // load value of variable inside loader function
  std::stringstream envVarNameBuffer;
  envVarNameBuffer
    << "LLVMREPAIR_INSTRUCTION_MUTANT_"
    << sourceMapping->get(instruction)->getID();

  auto *envVarNamePtr = builder.CreateGlobalStringPtr(envVarNameBuffer.str());

  // use getenv to fetch optional mutant ID
  auto *getenvFunction = module.getFunction("getenv");
  std::vector<llvm::Value*> getenvArgs = {
    llvm::ConstantExpr::getInBoundsGetElementPtr(i8Type, envVarNamePtr, zeroConstant)
  };
  auto *getenvResult = builder.CreateCall(getenvFunction->getFunctionType(), getenvFunction, getenvArgs, "");

  // create new block for nonnull_getenv
  // - calls strtol to obtain mutant ID
  // - stores result in global variable
  // - branch unconditionally to successor
  auto *nonnullBlock = llvm::BasicBlock::Create(context, "", loaderFunction, successorBlock);
  builder.SetInsertPoint(nonnullBlock);
  auto *strtolEnd = builder.CreateAlloca(i8PtrType);
  auto *strtolFunction = module.getFunction("strtol");
  std::vector<llvm::Value*> strtolArgs = {
    getenvResult,
    strtolEnd,
    builder.getInt32(10)
  };
  auto *strtolResult = builder.CreateCall(
    strtolFunction->getFunctionType(),
    strtolFunction,
    strtolArgs,
    ""
  );
  builder.CreateStore(strtolResult, global);

  // DEBUGGING: call stderr to show that mutation has been set successfully
  auto *printfFunction = module.getFunction("printf");
  auto *activationStrPtr = builder.CreateGlobalStringPtr("[LLVM-SUPERMUTATE] ACTIVATE MUTATION %d AT %s\n");
  std::vector<llvm::Value*> printfArgs = {
    llvm::ConstantExpr::getInBoundsGetElementPtr(i8Type, activationStrPtr, zeroConstant),
    strtolResult,
    llvm::ConstantExpr::getInBoundsGetElementPtr(i8Type, envVarNamePtr, zeroConstant)
  };
  builder.CreateCall(printfFunction->getFunctionType(), printfFunction, printfArgs, "");

  // continue to the successor block
  builder.CreateBr(successorBlock);

  // implement non-null branching
  // - remove the br instruction that was generated when splitting the entry block
  entryBlock->back().eraseFromParent();
  builder.SetInsertPoint(entryBlock);
  auto *nullCheckResult = builder.CreateICmpEQ(
    getenvResult,
    llvm::ConstantPointerNull::get(i8PtrType)
  );
  builder.CreateCondBr(nullCheckResult, successorBlock, nonnullBlock);

  return global;
}

void MutationEngine::prepareInstruction(llvm::Instruction *instruction) {
  llvm::IRBuilder<> builder(context);

  auto *originalBlock = instruction->getParent();
  auto *function = instruction->getFunction();

  // inject code to determine the mutation to use for this instruction at run-time
  auto *global = prepareInstructionLoader(instruction);

  // first of all, we need to create a new basic block for the original instruction
  // we don't want to move the original instruction at this point -- that would create problems
  // instead, we create a carbon copy of the instruction and place it inside a new
  // basic block
  //
  // https://stackoverflow.com/questions/43303943/in-llvm-ir-i-want-to-copy-a-set-of-instructions-and-paste-those-instructions-to
  auto *cloneBlock = llvm::BasicBlock::Create(context, "", function);
  auto *cloneInstruction = instruction->clone();
  instructionToClone.emplace(instruction, cloneInstruction);
  instructionToCloneBlock.emplace(instruction, cloneBlock);
  cloneBlock->getInstList().push_back(cloneInstruction);

  // create a switch instruction and insert immediately before the original instruction
  // - use global variable for switching
  // - load value of global variable into a register
  // - use basic block for the cloned original instruction as the default/fallback destination
  builder.SetInsertPoint(instruction);
  auto *globalValue = builder.CreateLoad(builder.getInt64Ty(), global);
  auto *switchInstruction = builder.CreateSwitch(globalValue, cloneBlock);
  instructionToSwitchMap.emplace(instruction, switchInstruction);

  // since our newly created switch instruction is a terminator, we need to move all of the
  // instructions following the original instruction into a new basic block
  //
  // for sake of convenience, we don't erase the original instruction until later
  //
  // NOTE: if the instruction being mutated is a terminator, then we don't need to introduce
  // a remainder block
  if (!instruction->isTerminator()) {
    auto *remainderBlock = originalBlock->splitBasicBlock(instruction);
    instructionToDestinationBlock.emplace(instruction, remainderBlock);
    originalBlock->getInstList().pop_back();

    // and now that we've generated the remainder block, we add a terminator to the clone
    // block (i.e., we get the clone block to jump unconditionally to the remainder block)
    llvm::BranchInst::Create(remainderBlock, cloneBlock);

    // inject a PHI node at the top of the remainder block
    auto *originalValue = llvm::cast<llvm::Value>(instruction);
    auto *clonedValue = llvm::cast<llvm::Value>(cloneInstruction);

    auto *phi = llvm::PHINode::Create(originalValue->getType(), 1, "", instruction);
    phi->addIncoming(clonedValue, cloneBlock);
    instructionToPhiMap.emplace(instruction, phi);
    instruction->replaceAllUsesWith(phi);
  }

  // NOTE: we don't remove the original instruction at this point
  // we wait until we're done with injecting mutations before we remove the original instruction
}

void MutationEngine::writeMutationTable(std::string const &filename) {
  auto j = nlohmann::json::array();
  size_t mutationID = 0;

  for (auto *mutation : mutations) {
    mutationID++;
    j.push_back(mutation->toJSON(mutationID, sourceMapping));
  }

  std::ofstream o(filename);
  o << std::setw(2) << j << std::endl;
}

} // llvmsupermutate
