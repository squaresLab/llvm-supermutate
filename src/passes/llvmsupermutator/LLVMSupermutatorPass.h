#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>

namespace llvmsupermutate {

struct LLVMSupermutatorPass : public llvm::ModulePass {
  static char ID;

  LLVMSupermutatorPass() : ModulePass(ID) {}

  bool runOnModule(llvm::Module &module) override;
};

} // llvmsupermutate
