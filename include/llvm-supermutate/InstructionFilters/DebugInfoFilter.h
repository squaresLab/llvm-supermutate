#pragma once

#include "../InstructionFilter.h"
#include "../Mapping/LLVMToSourceMapping.h"


namespace llvmsupermutate {

/**
 * This filter ensures that only instructions with source-mapping information are mutated
 */
class DebugInfoFilter : public InstructionFilter {
public:
  DebugInfoFilter(LLVMToSourceMapping *sourceMapping) : sourceMapping(sourceMapping) {}
  ~DebugInfoFilter(){};

  bool isMutable(llvm::Instruction const &instruction) const override {
    return sourceMapping->hasInfo(const_cast<llvm::Instruction*>(&instruction));
  }

  std::string describe() const override {
    return "DebugInfoFilter";
  }

private:
  LLVMToSourceMapping *sourceMapping;
};

} // llvmsupermutate
