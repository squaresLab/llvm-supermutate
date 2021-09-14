#pragma once

#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/GlobalVariable.h>

#include <nlohmann/json.hpp>

namespace llvmsupermutate {

class GlobalInfo {
public:
  GlobalInfo(size_t id, llvm::GlobalVariable *global);

  size_t getID() const;
  std::string getName() const;
  llvm::GlobalVariable* getGlobalVariable() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::GlobalVariable *global;

}; // GlobalInfo

} // llvmsupermutate
