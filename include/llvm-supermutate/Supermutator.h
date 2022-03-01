#pragma once

#include <string>
#include <vector>

#include "Mapping/LLVMToSourceMapping.h"
#include "InstructionFilter.h"
#include "MutationEngine.h"


namespace llvmsupermutate {

class Supermutator {
public:
  Supermutator(llvm::Module &module, std::string const &outputFilename);

  /** Adds a new mutator */
  void addMutator(InstructionMutator *mutator);

  /** Adds a new instruction filter */
  void addFilter(std::unique_ptr<InstructionFilter> filter);

  /** Determines whether a given instruction is considered to be mutable */
  bool isMutable(llvm::Instruction const &instruction) const;

  /** Returns the associated mutation engine */
  MutationEngine& getMutationEngine() { return mutationEngine; }

  /** Constructs the supermutant */
  void run();

private:
  llvm::Module &module;
  LLVMToSourceMapping *sourceMapping;
  MutationEngine mutationEngine;

  /** The name of the file to which the supermutated bitcode should be written */
  std::string outputFilename;

  /** Used to determine which instructions are eligible for supermutation */
  std::vector<std::unique_ptr<InstructionFilter>> filters;
};

} // llvmsupermutate
