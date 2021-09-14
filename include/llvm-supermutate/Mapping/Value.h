#pragma once

#include <string>

#include <llvm/IR/Value.h>

#include <nlohmann/json.hpp>


namespace llvmsupermutate {

class ValueInfo {
public:
  ValueInfo(size_t id, llvm::Value *value);

  size_t getID() const;
  llvm::Value* getValue() const;

  std::string describe() const;

  bool isInstruction() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::Value *value;

}; // ValueInfo

} // llvmsupermutate
