#include "LLVMSupermutatorPass.h"

#include <llvm/IR/InstIterator.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>
#include <llvm-supermutate/MutationEngine.h>


static llvm::cl::opt<std::string> mutatedFilename(
  "mutated-filename",
  llvm::cl::desc("The name of file to which the supermutated bitcode should be written."),
  llvm::cl::value_desc("filename"),
  llvm::cl::Required
);

using namespace llvm;
using namespace llvmsupermutate;

bool llvmsupermutate::LLVMSupermutatorPass::runOnModule(Module &module) {
  auto *mapping = LLVMToSourceMapping::build(module);

  // construct the mutation engine
  MutationEngine mutationEngine(module, mapping);

  // TODO add mutators
  // mutationEngine.addMutator(new ICmpMutator(mutationEngine));
  // mutationEngine.addMutator(new BinOpcodeMutator(mutationEngine));
  // mutationEngine.addMutator(new GuardStatementMutator(mutationEngine, fixLocalization));

  // FIXME decide which instructions should be mutated

  // write the mutation table to disk
  mutationEngine.writeMutationTable("mutations.json");

  // write the mutated bitcode to disk
  mutationEngine.writeMutatedBitcode(mutatedFilename);

  return true;
}

char llvmsupermutate::LLVMSupermutatorPass::ID = 0;
static RegisterPass<llvmsupermutate::LLVMSupermutatorPass> X(
  "llvmsupermutate", "Repairs a given vulnerability in a LLVM bitcode file", false, true
);

static RegisterStandardPasses Y(
  PassManagerBuilder::EP_OptimizerLast,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new llvmsupermutate::LLVMSupermutatorPass());
  }
);
