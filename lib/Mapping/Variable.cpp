#include <llvm-supermutate/Mapping/Variable.h>

using json = nlohmann::json;

namespace llvmsupermutate {

VariableInfo::VariableInfo(size_t id, llvm::DIVariable *variable, FunctionInfo *functionInfo)
  : id(id), variable(variable), functionInfo(functionInfo)
{ }

size_t VariableInfo::getID() const {
  return id;
}

FunctionInfo* VariableInfo::getFunctionInfo() const {
  return functionInfo;
}

FileInfo* VariableInfo::getFileInfo() const {
  return functionInfo->getFileInfo();
}

std::string VariableInfo::getName() const {
  return variable->getName().str();
}

json VariableInfo::toJSON() const {
  return {
    {"id", id},
    {"name", getName()},
    {"function-id", getFunctionInfo()->getId()}
  };
}

} // llvmsupermutate
