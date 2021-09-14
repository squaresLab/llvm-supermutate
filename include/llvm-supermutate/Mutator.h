#pragma once

#include <string>

#include <llvm/IR/Instruction.h>

namespace llvmsupermutate {

class InstructionMutator {
public:
  virtual ~InstructionMutator(){};
  virtual std::string const getName() const = 0;
  virtual void mutate(llvm::Instruction *original) = 0;
};

} // llvmsupermutate
