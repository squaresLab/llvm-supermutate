#pragma once

#include <set>
#include <string>

#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>

#include "Supermutator.h"

namespace llvmsupermutate {

class SupermutatorConfig {
public:
  // loads a configuration from a given file
  static SupermutatorConfig load(std::string const &filename);

  // loads a configuration from a given JSON document
  static SupermutatorConfig load(nlohmann::json j);

  // constructs the LLVM supermutator described by this config
  Supermutator build(llvm::Module &module) const;

private:
  SupermutatorConfig()
  : restrictToFiles(),
    restrictToFunctions()
  {}

  // the set of source files that mutation should be restricted to
  // TODO given as absolute paths?
  std::set<std::string> restrictToFiles;

  // the set of functions that mutation should be restricted to, given by their names
  std::set<std::string> restrictToFunctions;
};

}
