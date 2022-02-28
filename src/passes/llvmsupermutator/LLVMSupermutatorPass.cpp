#include "LLVMSupermutatorPass.h"

#include <llvm/IR/InstIterator.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>
#include <llvm-supermutate/MutationEngine.h>
#include <llvm-supermutate/Mutators.h>


static llvm::cl::opt<std::string> mutatedFilename(
  "mutated-filename",
  llvm::cl::desc("The name of file to which the supermutated bitcode should be written."),
  llvm::cl::value_desc("filename"),
  llvm::cl::Required
);

static llvm::cl::opt<std::string> targetFunctionName(
  "mutate-function",
  llvm::cl::desc("The name of the function that should be supermutated."),
  llvm::cl::value_desc("function name"),
  llvm::cl::Required
);

using namespace llvm;
using namespace llvmsupermutate;

bool llvmsupermutate::LLVMSupermutatorPass::runOnModule(Module &module) {
  auto *mapping = LLVMToSourceMapping::build(module);

  // construct the mutation engine
  MutationEngine mutationEngine(module, mapping);

  // TODO allow users to specify operators and their settings via JSON config file
  // mutationEngine.addMutator(new ICmpMutator(mutationEngine));
  mutationEngine.addMutator(new BinOpcodeMutator(mutationEngine));

  // TODO decide which instructions should be mutated
  // for now, we simply mutate all instructions in a given function
  //
  // TODO idea: add the concept of InstructionFilters
  // - used to decide which instructions should be mutated
  // - simple filters can be chained together
  // - e.g., ignore certain instructions
  // - e.g., restrict to certain functions

  // write the mutation table to disk
  mutationEngine.writeMutationTable("mutations.json");

  // write the mutated bitcode to disk
  mutationEngine.writeMutatedBitcode(mutatedFilename);

  return true;
}

char llvmsupermutate::LLVMSupermutatorPass::ID = 0;
static RegisterPass<llvmsupermutate::LLVMSupermutatorPass> X(
  "llvmsupermutate", "Generates a supermutant from a given LLVM bitcode file", false, true
);

static RegisterStandardPasses Y(
  PassManagerBuilder::EP_OptimizerLast,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new llvmsupermutate::LLVMSupermutatorPass());
  }
);
