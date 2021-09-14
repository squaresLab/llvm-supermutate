#pragma once

#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include <nlohmann/json.hpp>

#include "File.h"
#include "Function.h"

namespace llvmsupermutate {

class VariableInfo {
public:
  VariableInfo(size_t id, llvm::DIVariable *variable, FunctionInfo *functionInfo);

  size_t getID() const;
  llvm::DIVariable* getVariable() const;
  FunctionInfo* getFunctionInfo() const;
  FileInfo* getFileInfo() const;
  std::string getName() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::DIVariable *variable;
  FunctionInfo *functionInfo;

}; // VariableInfo

} // llvmsupermutate
