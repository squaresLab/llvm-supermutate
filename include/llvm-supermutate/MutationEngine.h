#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include "Mapping/LLVMToSourceMapping.h"
#include "Mutator.h"
#include "Mutation.h"


namespace llvmsupermutate {

class MutationEngine {
public:
  MutationEngine(llvm::Module &module, LLVMToSourceMapping *sourceMapping);
  ~MutationEngine();

  /** Injects a given mutation into the associated LLVM module */
  void inject(InstructionMutation *mutation);

  /** Determines whether a given instruction has been mutated */
  bool hasBeenMutated(llvm::Instruction *instruction) const;

  /** Uses the associated mutators to mutate a given instruction */
  void mutate(llvm::Instruction *instruction);

  /** Writes details of the injected mutations to a JSON file */
  void writeMutationTable(std::string const &filename);

  /** Writes the mutated bitcode to a given file */
  void writeMutatedBitcode(std::string const &filename);

  /** Registers a given mutator with this engine */
  void addMutator(InstructionMutator *mutator);

private:
  size_t nextMutantId = 1;
  llvm::Module &module;
  llvm::LLVMContext &context;
  LLVMToSourceMapping *sourceMapping;
  llvm::Function *loaderFunction;

  // TODO prefer unique_ptr
  std::vector<InstructionMutator*> mutators;
  // TODO prefer unique_ptr; yes
  std::vector<InstructionMutation*> mutations;

  std::unordered_map<llvm::Instruction*, llvm::Instruction*> instructionToClone;
  std::unordered_map<llvm::Instruction*, llvm::SwitchInst*> instructionToSwitchMap;
  std::unordered_map<llvm::Instruction*, llvm::PHINode*> instructionToPhiMap;
  std::unordered_map<llvm::Instruction*, llvm::BasicBlock*> instructionToDestinationBlock;
  std::unordered_map<llvm::Instruction*, llvm::BasicBlock*> instructionToCloneBlock;

  void prepare();
  void prepareInstruction(llvm::Instruction *instruction);
  llvm::Value* prepareInstructionLoader(llvm::Instruction *instruction);
}; // MutationEngine

} // llvmsupermutate
