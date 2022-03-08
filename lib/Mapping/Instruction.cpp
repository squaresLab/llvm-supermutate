#include <llvm-supermutate/Mapping/Instruction.h>

using json = nlohmann::json;

namespace llvmsupermutate {

InstructionInfo::InstructionInfo(size_t id, llvm::Instruction *instruction, FunctionInfo *functionInfo)
  : id(id), instruction(instruction), functionInfo(functionInfo)
{ }

size_t InstructionInfo::getID() const {
  return id;
}

llvm::Instruction* InstructionInfo::getInstruction() const {
  return instruction;
}

FunctionInfo* InstructionInfo::getFunctionInfo() const {
  return functionInfo;
}

FileInfo* InstructionInfo::getFileInfo() const {
  return functionInfo->getFileInfo();
}

const llvm::DebugLoc& InstructionInfo::getDebugLoc() const {
  return instruction->getDebugLoc();
}

llvm::Optional<std::pair<unsigned, unsigned>> InstructionInfo::getLineCol() const {
  auto &debugLoc = getDebugLoc();
  if  (debugLoc.isImplicitCode()) {
    return llvm::Optional<std::pair<unsigned, unsigned>>();
  }

  return llvm::Optional<std::pair<unsigned, unsigned>>({debugLoc.getLine(), debugLoc.getCol()});
}

std::string InstructionInfo::getFilename() const {
  return getFileInfo()->getFilename();
}

std::string InstructionInfo::getDirectory() const {
  return getFileInfo()->getDirectory();
}

bool InstructionInfo::isImplicit() const {
  return !getLineCol().hasValue();
}

json InstructionInfo::toJSON() const {
  auto maybeLineCol = getLineCol();
  bool implicit = !maybeLineCol.hasValue();

  std::string description;
  llvm::raw_string_ostream os(description);
  getInstruction()->print(os);

  json j = {
    {"id", id},
    {"function-id", functionInfo->getId()},
    {"implicit", implicit},
    {"description", description}
  };

  if (!implicit) {
    auto line_and_col = maybeLineCol.getValue();
    j["line"] = line_and_col.first;
    j["column"] = line_and_col.second;
  }

  return j;
}

} // llvmsupermutate
