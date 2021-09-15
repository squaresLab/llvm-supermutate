#include <llvm-supermutate/Mapping/Global.h>

using json = nlohmann::json;

namespace llvmsupermutate {

GlobalInfo::GlobalInfo(size_t id, llvm::GlobalVariable *global)
  : id(id), global(global)
{ }

size_t GlobalInfo::getID() const {
  return id;
}

std::string GlobalInfo::getName() const {
  return global->getName().str();
}

llvm::GlobalVariable* GlobalInfo::getGlobalVariable() const {
  return global;
}

json GlobalInfo::toJSON() const {
  json j = {
    {"id", id},
    {"name", getName()},
  };

  return j;
}

} // llvmsupermutate
