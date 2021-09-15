#include <llvm-supermutate/Mutation.h>

namespace llvmsupermutate {

nlohmann::json InstructionMutation::toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const {
  // FIXME maintain a pointer to InstructionInfo
  nlohmann::json j = {
    {"id", id},
    {"instruction", sourceMapping->get(getOriginal())->getID()},
    {"operator", getMutatorName()}
  };
  return j;
}

} // llvmsupermutate
