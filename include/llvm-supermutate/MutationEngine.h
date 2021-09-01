#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include "LLVMToSourceMapping.h"
#include "Mutator.h"


namespace llvmrepair {


class MutationEngine {
public:
  MutationEngine(llvm::Module &module, llvmtosource::IRSourceMapping *sourceMapping);
  ~MutationEngine();

  void inject(InstructionMutation *mutation);

  /** Determines whether a given instruction has been mutated. */
  bool hasBeenMutated(llvm::Instruction *instruction) const;

  /** Uses the associated mutators to mutate a given instruction. */
  void mutate(llvm::Instruction *instruction);

  /** Writes details of the injected mutations to a JSON file **/
  void writeMutationTable(std::string filename);

  /** Writes the mutated bitcode to a given file. **/
  void writeMutatedBitcode(std::string filename);

  void addMutator(InstructionMutator *mutator);

private:
  size_t nextMutantId = 1;
  llvm::Module &module;
  llvm::LLVMContext &context;
  LLVMToSourceMapping *sourceMapping;
  llvm::Function *loaderFunction;

  std::vector<InstructionMutator*> mutators;
  // TODO prefer unique_ptr?
  std::vector<InstructionMutation*> mutations;

  std::unordered_map<llvm::Instruction*, llvm::Instruction*> instructionToClone;
  std::unordered_map<llvm::Instruction*, llvm::SwitchInst*> instructionToSwitchMap;
  std::unordered_map<llvm::Instruction*, llvm::PHINode*> instructionToPhiMap;
  std::unordered_map<llvm::Instruction*, llvm::BasicBlock*> instructionToDestinationBlock;

  void prepare();
  void prepareInstruction(llvm::Instruction *instruction);
  llvm::Value* prepareInstructionLoader(llvm::Instruction *instruction);
}; // MutationEngine


} // llvmrepair
