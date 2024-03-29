#pragma once

#include <string>
#include <vector>

#include "Mapping/LLVMToSourceMapping.h"
#include "InstructionFilter.h"
#include "MutationEngine.h"


namespace llvmsupermutate {

class Supermutator {
public:
  Supermutator(
    llvm::Module &module,
    std::set<std::string> const &restrictToFiles,
    std::set<std::string> const &restrictToFunctions
  );

  /** Adds a new mutator */
  void addMutator(InstructionMutator *mutator);

  /** Adds a new instruction filter */
  void addFilter(std::unique_ptr<InstructionFilter> filter);

  /** Determines whether a given instruction is considered to be mutable */
  bool isMutable(llvm::Instruction const &instruction) const;

  /** Determines whether a given function is considered to be mutable */
  bool isMutable(llvm::Function const &function) const;

  /** Returns the associated mutation engine */
  MutationEngine& getMutationEngine() { return mutationEngine; }

  /** Returns the associated source mapping */
  LLVMToSourceMapping* getSourceMapping() { return sourceMapping; }

  /** Constructs the supermutant */
  void run();

private:
  llvm::Module &module;
  std::set<std::string> restrictToFiles;
  std::set<std::string> restrictToFunctions;
  LLVMToSourceMapping *sourceMapping;
  MutationEngine mutationEngine;

  /** The name of the file to which the supermutated bitcode should be written */
  std::string outputFilename;

  /** Used to determine which instructions are eligible for supermutation */
  std::vector<std::unique_ptr<InstructionFilter>> filters;
};

} // llvmsupermutate
