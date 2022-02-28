#include <llvm-supermutate/Supermutator.h>

namespace llvmsupermutate {

void Supermutator::addMutator(InstructionMutator *mutator) {
  mutationEngine.addMutator(mutator);
}

void Supermutator::addFilter(std::unique_ptr<InstructionFilter> filter) {
  filters.emplace_back(std::move(filter));
}

bool Supermutator::isMutable(llvm::Instruction const &instruction) const {
  for (auto &&filter : filters) {
    if (!filter->isMutable(instruction))
      return false;
  }
  return true;
}

void Supermutator::run() {
  for (llvm::Function &function : module) {
    for (auto &block : function) {
      for (auto &instruction : block) {
        if (isMutable(instruction))
          mutationEngine.mutate(&instruction);
      }
    }
  }

  mutationEngine.writeMutationTable("mutations.json");
  mutationEngine.writeMutatedBitcode(outputFilename);
}

} // llvmsupermutate
