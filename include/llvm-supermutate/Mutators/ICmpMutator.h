#pragma once

#include "../MutationEngine.h"
#include "../Mutator.h"

#include <llvm/IR/Instructions.h>


namespace llvmsupermutate {

class ICmpMutator : public InstructionMutator {
public:
  ICmpMutator(MutationEngine &mutationEngine)
    : mutationEngine(mutationEngine)
  {};
  ~ICmpMutator(){};

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
      ICmpMutator *mutator,
      llvm::Instruction *original,
      llvm::CmpInst::Predicate predicate
    ) : mutator(mutator),
        original(original),
        predicate(predicate)
    { }

    InstructionMutator *mutator;
    llvm::Instruction *original;
    llvm::CmpInst::Predicate predicate;

    friend class ICmpMutator;
  };

  std::string const getName() const override {
    return "ICmpMutator";
  }
  void mutate(llvm::Instruction *original) override;

private:
  MutationEngine &mutationEngine;

  void mutate(llvm::ICmpInst *original);
};

}
