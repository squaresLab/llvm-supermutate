#include <llvm-supermutate/InstructionFilters.h>
#include <llvm-supermutate/Mutators.h>
#include <llvm-supermutate/SupermutatorConfig.h>

#include <spdlog/spdlog.h>

#include <experimental/filesystem>
#include <fstream>

namespace llvmsupermutate {

SupermutatorConfig SupermutatorConfig::load(std::string const &filename) {
  if (!std::experimental::filesystem::exists(filename)) {
    spdlog::error("configuration file not found: {}", filename);
    exit(1);
  }

  nlohmann::json j;
  std::ifstream input(filename);
  input >> j;
  return load(j);
}

SupermutatorConfig SupermutatorConfig::load(nlohmann::json j) {
  SupermutatorConfig config;

  // TODO produce an error (or warning) if an unrecognized field appears in the config

  if (j.contains("filenames")) {
    spdlog::debug("mutation will be restricted to certain files");
    for (auto& filename : j["filenames"]) {
      spdlog::debug("allowing file to be mutated: {}", filename);
      config.restrictToFiles.emplace(filename);
    }
  }

  if (j.contains("functions")) {
    spdlog::debug("mutation will be restricted to certain functions");
    for (auto& function : j["functions"]) {
      spdlog::debug("allowing function to be mutated: {}", function);
      config.restrictToFunctions.emplace(function);
    }
  }

  return config;
}

Supermutator SupermutatorConfig::build(llvm::Module &module) const {
  auto supermutator = Supermutator(
    module,
    restrictToFiles,
    restrictToFunctions
  );

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
  supermutator.addMutator(new ICmpMutator(supermutator.getMutationEngine()));
  supermutator.addMutator(new LoadMutator(supermutator.getMutationEngine()));

  return supermutator;
}

}
