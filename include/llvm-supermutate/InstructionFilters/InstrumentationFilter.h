#pragma once

#include "../InstructionFilter.h"


namespace llvmsupermutate {

/**
 * This filter prevents supermutation instrumentation from being subject to mutation.
 */
class InstrumentationFilter : public InstructionFilter {
public:
  InstrumentationFilter(){}
  ~InstrumentationFilter(){};

  bool isMutable(llvm::Instruction const &instruction) const override {
    if (auto *callInst = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
      auto *calledFunction = callInst->getCalledFunction();
      if (calledFunction == nullptr) {
        return false;
      }
      return calledFunction->getName().str() != "__llvm_mutate_load_mutations__";
    }
    return true;
  }
};

} // llvmsupermutate
