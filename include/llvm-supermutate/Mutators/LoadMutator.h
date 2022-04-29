#pragma once

#include "../MutationEngine.h"
#include "../Mutator.h"

#include <llvm/IR/Instructions.h>


namespace llvmsupermutate {

class LoadMutator : public InstructionMutator {
public:
  LoadMutator(MutationEngine &mutationEngine)
    : mutationEngine(mutationEngine)
  {};
  ~LoadMutator(){};

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
    nlohmann::json toJSON(
      size_t id,
      LLVMToSourceMapping *sourceMapping
    ) const override;

  private:
    Mutation(
      LoadMutator *mutator,
      llvm::Instruction *original,
      llvm::Value *pointerOperand
    ) : mutator(mutator),
        original(original),
        pointerOperand(pointerOperand)
    { }

    InstructionMutator *mutator;
    llvm::Instruction *original;
    llvm::Value *pointerOperand;

    friend class LoadMutator;
  };

  std::string const getName() const override {
    return "LoadMutator";
  }
  void mutate(llvm::Instruction *original) override;

private:
  MutationEngine &mutationEngine;

  void mutate(llvm::LoadInst *original);
};

}
