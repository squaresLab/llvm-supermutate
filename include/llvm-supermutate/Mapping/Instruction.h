#pragma once

#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Instruction.h>

#include <nlohmann/json.hpp>

#include "File.h"
#include "Function.h"

namespace llvmsupermutate {

class InstructionInfo {
public:
  InstructionInfo(size_t id, llvm::Instruction *instruction, FunctionInfo *functionInfo);

  size_t getID() const;
  FunctionInfo* getFunctionInfo() const;
  FileInfo* getFileInfo() const;
  llvm::Instruction* getInstruction() const;
  const llvm::DebugLoc& getDebugLoc() const;
  llvm::Optional<std::pair<unsigned, unsigned>> getLineCol() const;
  std::string getFilename() const;
  bool isImplicit() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::Instruction *instruction;
  FunctionInfo *functionInfo;

}; // InstructionInfo

} // llvmsupermutate
