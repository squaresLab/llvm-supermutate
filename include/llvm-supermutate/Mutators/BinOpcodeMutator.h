#pragma once

#include "../MutationEngine.h"
#include "../Mutator.h"

#include <llvm/IR/Instructions.h>

namespace llvmsupermutate {

class BinOpcodeMutator : public InstructionMutator {
public:
  BinOpcodeMutator(MutationEngine &mutationEngine)
    : mutationEngine(mutationEngine)
  {}
  ~BinOpcodeMutator(){};

  class Mutation : public InstructionMutation {
  public:
    void inject(
      llvm::BasicBlock *mutantBlock,
      llvm::BasicBlock *cloneBlock,
      llvm::BasicBlock *remainderBlock,
      llvm::PHINode *phi
    ) override;

    llvm::Instruction* getOriginal() const override {
      return original;
    }
    InstructionMutator* getMutator() const override {
      return mutator;
    }
    nlohmann::json toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const override;

  private:
    Mutation(
      BinOpcodeMutator *mutator,
      llvm::Instruction *original,
      llvm::Instruction::BinaryOps opcode
    ) : mutator(mutator),
        original(original),
        opcode(opcode)
    { }

    InstructionMutator *mutator;
    llvm::Instruction *original;
    llvm::Instruction::BinaryOps opcode;

    friend class BinOpcodeMutator;
  };

  std::string const getName() const override {
    return "BinOpcodeMutator";
  }

  void mutate(llvm::Instruction *original) override;

private:
  MutationEngine &mutationEngine;

  void mutate(llvm::BinaryOperator *original);
  void mutateIntOpcode(llvm::BinaryOperator *original);
  void mutateFloatOpcode(llvm::BinaryOperator *original);
};

} // llvmsupermutate
