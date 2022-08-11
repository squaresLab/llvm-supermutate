#include "LLVMSupermutatorPass.h"

#include <spdlog/spdlog.h>

#include <llvm/IR/InstIterator.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>
#include <llvm-supermutate/SupermutatorConfig.h>


static llvm::cl::opt<std::string> configurationFile(
  "supermutate-config",
  llvm::cl::desc("The configuration file that should be used by LLVM supermutate."),
  llvm::cl::value_desc("filename"),
  llvm::cl::Required
);

using namespace llvm;
using namespace llvmsupermutate;

bool llvmsupermutate::LLVMSupermutatorPass::runOnModule(Module &module) {
  spdlog::set_level(spdlog::level::debug);
  auto config = SupermutatorConfig::load(configurationFile);
  auto supermutator = config.build(module);
  supermutator.run();
  return true;
}

char llvmsupermutate::LLVMSupermutatorPass::ID = 0;
static RegisterPass<llvmsupermutate::LLVMSupermutatorPass> X(
  "supermutate", "Generates a supermutant from a given LLVM bitcode file", false, true
);

static RegisterStandardPasses Y(
  PassManagerBuilder::EP_OptimizerLast,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new llvmsupermutate::LLVMSupermutatorPass());
  }
);
