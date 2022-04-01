#pragma once

#include <llvm/IR/Instruction.h>

namespace llvmsupermutate {

class InstructionFilter {
public:
  virtual ~InstructionFilter(){}
  virtual bool isMutable(llvm::Instruction const &instruction) const = 0;
};

} // llvmsupermutate
