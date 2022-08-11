#pragma once

#include <string>
#include <unordered_set>

#include "../InstructionFilter.h"


namespace llvmsupermutate {

class FunctionFilter : public InstructionFilter {
public:
  FunctionFilter() : restrictToFunctions() {}
  FunctionFilter(std::unordered_set<std::string> &restrictToFunctions)
    : restrictToFunctions(restrictToFunctions)
  {}
  ~FunctionFilter(){};

  void allowFunction(std::string const &name) {
    restrictToFunctions.emplace(name);
  }

  bool isMutable(llvm::Function const *function) const {
    return restrictToFunctions.find(function->getName().str()) != restrictToFunctions.end();
  }

  bool isMutable(llvm::Instruction const &instruction) const override {
    return isMutable(instruction.getFunction());
  }

  std::string describe() const override {
    return "FunctionFilter";
  }

private:
  std::unordered_set<std::string> restrictToFunctions;
};

} // llvmsupermutate
