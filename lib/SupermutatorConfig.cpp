#include <llvm-supermutate/InstructionFilters.h>
#include <llvm-supermutate/Mutators.h>
#include <llvm-supermutate/SupermutatorConfig.h>

#include <fstream>

namespace llvmsupermutate {

SupermutatorConfig SupermutatorConfig::load(std::string const &filename) {
  nlohmann::json j;
  std::ifstream input(filename);
  input >> j;
  return load(j);
}

SupermutatorConfig SupermutatorConfig::load(nlohmann::json j) {
  SupermutatorConfig config;

  if (j.contains("filenames")) {
    for (auto& filename : j["filenames"]) {
      config.restrictToFiles.emplace(filename);
    }
  }

  if (j.contains("functions")) {
    for (auto& function : j["functions"]) {
      config.restrictToFunctions.emplace(function);
    }
  }

  return config;
}

Supermutator SupermutatorConfig::build(llvm::Module &module) const {
  auto supermutator = Supermutator(module);

  // TODO specify custom output directory

  // add instruction filters
  if (!restrictToFunctions.empty()) {
    auto functionFilter = std::make_unique<FunctionFilter>();
    for (auto& function : restrictToFunctions) {
      functionFilter->allowFunction(function);
    }
    supermutator.addFilter(std::move(functionFilter));
  }

  // TODO add mutators
  supermutator.addMutator(new BinOpcodeMutator(supermutator.getMutationEngine()));

  return supermutator;
}

}
