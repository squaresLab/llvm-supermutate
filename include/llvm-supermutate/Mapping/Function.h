#pragma once

#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include <nlohmann/json.hpp>

#include "File.h"

namespace llvmsupermutate {

class FunctionInfo {
public:
  FunctionInfo(size_t id, FileInfo *fileInfo, llvm::Function *function);

  size_t getId() const;
  llvm::Function* getFunction() const;
  FileInfo* getFileInfo() const;
  std::string getName() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::Function *function;
  FileInfo *fileInfo;

}; // FunctionInfo

} // llvmsupermutate
