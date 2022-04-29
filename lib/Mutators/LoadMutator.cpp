#include <llvm-supermutate/Mutators/LoadMutator.h>
#include <llvm-supermutate/Utils.h>

#include <vector>

#include <llvm/IR/IRBuilder.h>


namespace llvmsupermutate {

void LoadMutator::mutate(llvm::Instruction* original) {
  if (llvm::isa_and_nonnull<llvm::LoadInst>(original)) {
    mutate(llvm::cast<llvm::LoadInst>(original));
  }
}

void LoadMutator::mutate(llvm::LoadInst *original) {
  auto *expectedType = original->getPointerOperandType()->getPointerElementType();
  for (auto *value : findReachingValuesWithType(original, expectedType)) {
    // allow allocas and globals
    if (!llvm::isa_and_nonnull<llvm::AllocaInst>(value)) {
      continue;
    }
    // disallow use of retval!
    if (value->getName() == "retval") {
      continue;
    }
    mutationEngine.inject(new Mutation(this, original, value));
  }
}

void LoadMutator::Mutation::inject(
  llvm::BasicBlock *mutantBlock,
  llvm::BasicBlock *cloneBlock,
  llvm::BasicBlock *remainderBlock,
  llvm::PHINode *phi
) {
  llvm::IRBuilder<> builder(mutantBlock);
  auto *replacement = builder.CreateLoad(
    original->getType(),
    pointerOperand
  );
  phi->addIncoming(replacement, mutantBlock);
  builder.CreateBr(remainderBlock);
}

nlohmann::json LoadMutator::Mutation::toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const {
  nlohmann::json j = InstructionMutation::toJSON(id, sourceMapping);
  return j;
}

}
