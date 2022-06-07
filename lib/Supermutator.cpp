#include <llvm-supermutate/Supermutator.h>
#include <llvm-supermutate/InstructionFilters.h>

#include <unordered_set>


namespace llvmsupermutate {

Supermutator::Supermutator(llvm::Module &module)
: module(module),
  sourceMapping(LLVMToSourceMapping::build(module)),
  mutationEngine(module, sourceMapping),
  outputFilename("supermutant.bc")
{
  addFilter(std::make_unique<InstrumentationFilter>());
  addFilter(std::make_unique<DebugInfoFilter>(sourceMapping));
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
  llvm::outs() << "DEBUG: finding mutable instructions...\n";
  std::unordered_set<llvm::Instruction*> instructions;
  for (llvm::Function &function : module) {
    for (auto &block : function) {
      for (auto &instruction : block) {
        if (isMutable(instruction))
          instructions.insert(&instruction);
      }
    }
  }
  llvm::outs()
    << "DEBUG: found "
    << instructions.size()
    << " mutable instructions\n";

  llvm::outs() << "DEBUG: generating mutations...\n";
  for (auto instruction : instructions) {
    mutationEngine.mutate(instruction);
  }
  llvm::outs() << "DEBUG: finished generating mutations\n";

  // write llvm2source table
  mutationEngine.writeSourceMapping("source-mapping.json");
  mutationEngine.writeMutationTable("mutations.json");
  mutationEngine.writeMutatedBitcode(outputFilename);
}

} // llvmsupermutate
