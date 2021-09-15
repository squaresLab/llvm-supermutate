#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>

namespace llvmsupermutate {

struct LLVMToSourceMappingPass : public llvm::ModulePass {
  LLVMToSourceMapping *mapping;
  static char ID;

  LLVMToSourceMappingPass() : ModulePass(ID), mapping(nullptr) {}
  ~LLVMToSourceMappingPass();

  bool runOnModule(llvm::Module &module) override;

}; // LLVMToSourceMappingPass

} // llvmsupermutate
