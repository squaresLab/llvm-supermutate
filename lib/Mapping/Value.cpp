#include <llvm-supermutate/Mapping/Value.h>

#include <llvm/Support/raw_ostream.h>

using json = nlohmann::json;

namespace llvmsupermutate {

ValueInfo::ValueInfo(size_t id, llvm::Value *value)
  : id(id), value(value)
{ }

size_t ValueInfo::getID() const {
  return id;
}

std::string ValueInfo::describe() const {
  std::string result;
  llvm::raw_string_ostream buffer(result);
  value->print(buffer);
  return result;
}

bool ValueInfo::isInstruction() const {
  return llvm::isa<llvm::Instruction>(value);
}

llvm::Value* ValueInfo::getValue() const {
  return value;
}

json ValueInfo::toJSON() const {
  return {
    {"id", id},
    {"description", describe()}
  };
}

} // llvmsupermutate
