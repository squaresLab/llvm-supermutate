#include <llvm-supermutate/Mutators/ICmpMutator.h>

#include <vector>

#include <llvm/IR/IRBuilder.h>

namespace llvmsupermutate {

void ICmpMutator::mutate(llvm::Instruction* original) {
  if (llvm::isa_and_nonnull<llvm::ICmpInst>(original)) {
    mutate(llvm::cast<llvm::ICmpInst>(original));
  }
}

void ICmpMutator::mutate(llvm::ICmpInst *original) {
  // FCMP_OEQ
  // FCMP_OGT
  // FCMP_OGE
  // FCMP_OLT
  // FCMP_OLE
  // FCMP_ONE
  // FCMP_ORD
  // FCMP_UNO
  // FCMP_UEQ
  // FCMP_UGT
  // FCMP_UGE
  // FCMP_ULT
  // FCMP_ULE
  // FCMP_UNE
  // FCMP_TRUE

  // we can store these in the ICmpMutator instance?
  std::vector<llvm::CmpInst::Predicate> signedPredicates = {
    llvm::CmpInst::ICMP_EQ,
    llvm::CmpInst::ICMP_NE,
    llvm::CmpInst::ICMP_SGT,
    llvm::CmpInst::ICMP_SGE,
    llvm::CmpInst::ICMP_SLT,
    llvm::CmpInst::ICMP_SLE
  };
  std::vector<llvm::CmpInst::Predicate> unsignedPredicates = {
    llvm::CmpInst::ICMP_EQ,
    llvm::CmpInst::ICMP_NE,
    llvm::CmpInst::ICMP_UGT,
    llvm::CmpInst::ICMP_UGE,
    llvm::CmpInst::ICMP_ULT,
    llvm::CmpInst::ICMP_ULE
  };
  auto &predicates = original->isSigned() ? signedPredicates : unsignedPredicates;

  auto originalPredicate = original->getPredicate();
  for (auto replacementPredicate : predicates) {
    if (replacementPredicate == originalPredicate) {
      continue;
    }
    mutationEngine.inject(new Mutation(this, original, replacementPredicate));
  }
}

void ICmpMutator::Mutation::inject(
  llvm::BasicBlock *mutantBlock,
  llvm::BasicBlock *cloneBlock,
  llvm::BasicBlock *remainderBlock,
  llvm::PHINode *phi
) {
  llvm::IRBuilder<> builder(mutantBlock);
  auto *replacement = builder.CreateICmp(predicate, original->getOperand(0), original->getOperand(1));
  phi->addIncoming(replacement, mutantBlock);
  builder.CreateBr(remainderBlock);
}

nlohmann::json ICmpMutator::Mutation::toJSON(size_t id, LLVMToSourceMapping *sourceMapping) const {
  nlohmann::json j = InstructionMutation::toJSON(id, sourceMapping);
  j["predicate"] = llvm::ICmpInst::getPredicateName(predicate);
  return j;
}

}
