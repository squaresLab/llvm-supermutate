#include <llvm-supermutate/Mapping/Function.h>

#include <llvm/IR/Function.h>

using json = nlohmann::json;

namespace llvmsupermutate {

FunctionInfo::FunctionInfo(size_t id, FileInfo *fileInfo, llvm::Function *function)
  : id(id), function(function), fileInfo(fileInfo)
{ }

size_t FunctionInfo::getId() const {
  return id;
}

llvm::Function* FunctionInfo::getFunction() const {
  return function;
}

FileInfo* FunctionInfo::getFileInfo() const {
  return fileInfo;
}

std::string FunctionInfo::getName() const {
  return function->getName().str();
}

json FunctionInfo::toJSON() const {
  return {
    {"id", id},
    {"name", getName()},
    {"file-id", getFileInfo()->getId()},
  };
}

} // llvmsupermutate
