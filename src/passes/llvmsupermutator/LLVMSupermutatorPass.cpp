#include "LLVMSupermutatorPass.h"

#include <llvm/IR/InstIterator.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>
#include <llvm-supermutate/InstructionFilters.h>
#include <llvm-supermutate/Mutators.h>
#include <llvm-supermutate/Supermutator.h>


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
  auto supermutator = Supermutator(module, mutatedFilename);

  // TODO implement a JSON-based config file

  // add instruction filters
  auto functionFilter = std::make_unique<FunctionFilter>();
  functionFilter->allowFunction(targetFunctionName);
  supermutator.addFilter(std::move(functionFilter));

  // add mutators
  supermutator.addMutator(new BinOpcodeMutator(supermutator.getMutationEngine()));

  supermutator.run();

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
