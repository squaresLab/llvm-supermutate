#pragma once

#include <set>
#include <queue>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

namespace llvmsupermutate {

std::string getLlvmFileAbsPath(llvm::DIFile const *file);

std::string describeInstruction(llvm::Instruction const &instruction);

std::string describeInstruction(llvm::Instruction const *instruction);

std::set<llvm::BasicBlock*> transitivePredecessors(llvm::BasicBlock* block);

std::set<llvm::Value*> findReachingValues(llvm::Instruction *before);

std::set<llvm::Value*> findReachingValuesWithType(llvm::Instruction *before, llvm::Type *type);

}
