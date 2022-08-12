#include <llvm-supermutate/Supermutator.h>
#include <llvm-supermutate/InstructionFilters.h>
#include <llvm-supermutate/Utils.h>

#include <spdlog/spdlog.h>

#include <unordered_set>


namespace llvmsupermutate {

Supermutator::Supermutator(
  llvm::Module &module,
  std::set<std::string> const &restrictToFiles,
  std::set<std::string> const &restrictToFunctions
)
: module(module),
  restrictToFiles(restrictToFiles),
  restrictToFunctions(restrictToFunctions),
  sourceMapping(LLVMToSourceMapping::build(module, restrictToFiles)),
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
    if (!filter->isMutable(instruction)) {
      spdlog::debug("instruction removed by filter [{}]: {}", filter->describe(), describeInstruction(instruction));
      return false;
    }
  }
  return true;
}

bool Supermutator::isMutable(llvm::Function const &function) const {
  auto *subprogram = function.getSubprogram();
  if (subprogram == nullptr) {
    return false;
  }

  auto *file = subprogram->getFile();
  if (file == nullptr) {
    return false;
  }

  // is this file mutable?
  auto absoluteFilepath = getLlvmFileAbsPath(file);
  if (!restrictToFiles.empty() && restrictToFiles.find(absoluteFilepath) == restrictToFiles.end()) {
    return false;
  }

  // is this function mutable?
  auto name = function.getName().str();
  if (!restrictToFunctions.empty() && restrictToFunctions.find(name) == restrictToFunctions.end()) {
    spdlog::debug("skipping mutations to function: {}", name);
    return false;
  }
  
  return true;
}

void Supermutator::run() {
  spdlog::info("performing supermutation...");

  // write llvm2source table
  spdlog::debug("writing source mapping to disk...");
  mutationEngine.writeSourceMapping("source-mapping.json");
  spdlog::debug("wrote source mapping to disk");

  spdlog::debug("finding mutable instructions...");
  std::unordered_set<llvm::Instruction*> instructions;
  for (llvm::Function &function : module) {
    if (!isMutable(function)) {
      continue;
    }
    for (auto &block : function) {
      for (auto &instruction : block) {
        if (isMutable(instruction))
          instructions.insert(&instruction);
      }
    }
  }
  spdlog::debug("found {} mutable instructions", instructions.size());

  spdlog::debug("generating mutations...");
  for (auto instruction : instructions) {
    mutationEngine.mutate(instruction);
  }
  spdlog::debug("finished generating mutations");

  mutationEngine.writeMutationTable("mutations.json");
  mutationEngine.writeMutatedBitcode(outputFilename);
}

} // llvmsupermutate
