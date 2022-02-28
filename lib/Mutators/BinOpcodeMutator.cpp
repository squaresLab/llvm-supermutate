#include <llvm-supermutate/Mutators/BinOpcodeMutator.h>

#include <llvm/IR/IRBuilder.h>

namespace llvmsupermutate {

void BinOpcodeMutator::mutate(llvm::Instruction* original) {
  if (llvm::isa_and_nonnull<llvm::BinaryOperator>(original)) {
    mutate(llvm::cast<llvm::BinaryOperator>(original));
  }
}

void BinOpcodeMutator::mutate(llvm::BinaryOperator *original) {
  auto *resultType = original->getType();
  if (resultType->isIntOrIntVectorTy()) {
    mutateIntOpcode(original);
  } else {
    mutateFloatOpcode(original);
  }
}

void BinOpcodeMutator::mutateIntOpcode(llvm::BinaryOperator *original) {
  auto originalOpcode = original->getOpcode();
  static llvm::Instruction::BinaryOps opcodes[] = {
    llvm::Instruction::Add,
    llvm::Instruction::Sub,
    llvm::Instruction::Mul,
    llvm::Instruction::UDiv,
    llvm::Instruction::SDiv,
    llvm::Instruction::URem,
    llvm::Instruction::SRem,
    llvm::Instruction::Shl,
    llvm::Instruction::LShr,
    llvm::Instruction::AShr,
    llvm::Instruction::And,
    llvm::Instruction::Or,
    llvm::Instruction::Xor,
  };
  for (auto replacementOpcode : opcodes) {
    if (replacementOpcode == originalOpcode) {
      continue;
    }
    mutationEngine.inject(new Mutation(this, original, replacementOpcode));
  }
}

void BinOpcodeMutator::mutateFloatOpcode(llvm::BinaryOperator *original) {
  auto originalOpcode = original->getOpcode();
  static llvm::Instruction::BinaryOps opcodes[] = {
    llvm::Instruction::FAdd,
    llvm::Instruction::FSub,
    llvm::Instruction::FMul,
    llvm::Instruction::FDiv,
    llvm::Instruction::FRem
  };
  for (auto replacementOpcode : opcodes) {
    if (replacementOpcode == originalOpcode) {
      continue;
    }
    mutationEngine.inject(new Mutation(this, original, replacementOpcode));
  }
}

void BinOpcodeMutator::Mutation::inject(
  llvm::BasicBlock *mutantBlock,
  llvm::BasicBlock *cloneBlock,
  llvm::BasicBlock *remainderBlock,
  llvm::PHINode *phi
) {
  llvm::IRBuilder<> builder(mutantBlock);
  auto *replacement = builder.CreateBinOp(opcode, original->getOperand(0), original->getOperand(1));
  phi->addIncoming(replacement, mutantBlock);
  builder.CreateBr(remainderBlock);
}

nlohmann::json BinOpcodeMutator::Mutation::toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const {
  nlohmann::json j = InstructionMutation::toJSON(id, sourceMapping);
  j["opcode"] = llvm::BinaryOperator::getOpcodeName(opcode);
  return j;
}

} // llvmsupermutate
