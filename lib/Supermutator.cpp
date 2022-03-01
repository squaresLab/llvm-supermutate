#include <llvm-supermutate/Supermutator.h>
#include <llvm-supermutate/InstructionFilters.h>


namespace llvmsupermutate {

Supermutator::Supermutator(llvm::Module &module, std::string const &outputFilename)
: module(module),
  mutationEngine(module, LLVMToSourceMapping::build(module)),
  outputFilename(outputFilename)
{
  addFilter(std::make_unique<InstrumentationFilter>());
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
  // FIXME don't mutate the actual supermutation instrumentation!
  for (llvm::Function &function : module) {
    for (auto &block : function) {
      for (auto &instruction : block) {
        if (isMutable(instruction)) {
          // FIXME bad things are happening to the instruction address here!
          llvm::outs() << "[DEBUG] supermutating instruction: " << std::addressof(instruction) << "\n";
          mutationEngine.mutate(std::addressof(instruction));
        }
      }
    }
  }

  mutationEngine.writeMutationTable("mutations.json");
  mutationEngine.writeMutatedBitcode(outputFilename);
}

} // llvmsupermutate
