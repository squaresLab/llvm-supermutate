#include "LLVMToSourceMappingPass.h"

#include <spdlog/spdlog.h>

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

static cl::list<std::string> restrictToFiles(
  "restrict-mapping-to-files",
  cl::desc("The name of source files to which mapping should be restricted."),
  cl::value_desc("filename")
);

llvmsupermutate::LLVMToSourceMappingPass::~LLVMToSourceMappingPass() {
  if (mapping) {
    delete mapping;
  }
}

bool llvmsupermutate::LLVMToSourceMappingPass::runOnModule(Module &module) {
  std::set<std::string> restrictToFilesSet;
  for (auto &filename : restrictToFiles) {
    restrictToFilesSet.emplace(filename);
  }

  spdlog::set_level(spdlog::level::debug);
  spdlog::info("generating source mapping...");
  mapping = LLVMToSourceMapping::build(module, restrictToFilesSet);
  spdlog::info("saving source mapping to disk: {}", outputFilename);
  mapping->save(outputFilename);
  spdlog::info("saved source mapping to disk");
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
