#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <llvm/IR/Instruction.h>

#include "Mapping/LLVMToSourceMapping.h"
#include "Mutator.h"


namespace llvmsupermutate {

class InstructionMutation {
public:
  /** Returns the mutator that produced this mutation */
  virtual InstructionMutator* getMutator() const = 0;

  /** Returns the name of the mutator that produced this mutation */
  std::string const getMutatorName() const {
    return getMutator()->getName();
  }

  /** Returns a pointer to the original LLVM instruction that was mutated */
  virtual llvm::Instruction* getOriginal() const = 0;

  /** Carries out the process of injecting the replacement instruction(s) into a given basic block */
  virtual llvm::Instruction* injectReplacement(llvm::BasicBlock *block) = 0;

  /** Produces a JSON description of this mutation */
  virtual nlohmann::json toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const;

}; // InstructionMutation

} // llvmsupermutate
