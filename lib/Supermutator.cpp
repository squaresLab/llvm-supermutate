#include <llvm-supermutate/Supermutator.h>
#include <llvm-supermutate/InstructionFilters.h>


namespace llvmsupermutate {

Supermutator::Supermutator(llvm::Module &module, std::string const &outputFilename)
: module(module),
  sourceMapping(LLVMToSourceMapping::build(module)),
  mutationEngine(module, sourceMapping),
  outputFilename(outputFilename)
{
  addFilter(std::make_unique<InstrumentationFilter>());

  // TODO add filter that ensures instructions have source information!
}

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
  sourceMapping->dump();

  for (llvm::Function &function : module) {
    for (auto &block : function) {
      for (auto &instruction : block) {
        if (isMutable(instruction)) {
          mutationEngine.mutate(std::addressof(instruction));
        }
      }
    }
  }

  mutationEngine.writeMutationTable("mutations.json");
  mutationEngine.writeMutatedBitcode(outputFilename);
}

} // llvmsupermutate
