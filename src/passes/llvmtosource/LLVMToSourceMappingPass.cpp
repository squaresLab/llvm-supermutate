#include "LLVMToSourceMappingPass.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>

using namespace llvm;

static cl::opt<std::string> outputFilename(
  "mapping-filename",
  cl::desc("The name of file to which the IR-source mapping should be written."),
  cl::value_desc("filename"),
  cl::init("source-mapping.json")
);

llvmsupermutate::LLVMToSourceMappingPass::~LLVMToSourceMappingPass() {
  if (mapping) {
    delete mapping;
  }
}

bool llvmsupermutate::LLVMToSourceMappingPass::runOnModule(Module &module) {
  mapping = LLVMToSourceMapping::build(module);
  mapping->save(outputFilename);
  return false;
}

char llvmsupermutate::LLVMToSourceMappingPass::ID = 0;
static RegisterPass<llvmsupermutate::LLVMToSourceMappingPass> X(
  "llvmtosource", "Maps LLVM IR to corresponding C/C++ syntactical structures", false, true
);

static RegisterStandardPasses Y(
  PassManagerBuilder::EP_OptimizerLast,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new llvmsupermutate::LLVMToSourceMappingPass());
  }
);
