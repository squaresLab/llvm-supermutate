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

  // TODO: make this private?
  // class Mutation : public InstructionMutation {
  // public:
  //   // void inject(
  //   //   llvm::BasicBlock *mutantBlock,
  //   //   llvm::BasicBlock *cloneBlock,
  //   //   llvm::BasicBlock *remainderBlock,
  //   //   llvm::PHINode *phi
  //   // ) override;
  // };

  std::string const getName() override {
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
