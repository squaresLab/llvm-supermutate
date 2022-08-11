#pragma once

#include <set>
#include <queue>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>


namespace llvmsupermutate {

std::string describeInstruction(llvm::Instruction const &instruction) {
  std::string description;
  llvm::raw_string_ostream os(description);
  instruction.print(os);
  return description;
}

std::string describeInstruction(llvm::Instruction const *instruction) {
  return describeInstruction(*instruction);
}

std::set<llvm::BasicBlock*> transitivePredecessors(llvm::BasicBlock* block) {
  std::set<llvm::BasicBlock*> blocks;

  std::queue<llvm::BasicBlock*> q;
  for (auto *predecessor : llvm::predecessors(block)) {
    q.push(predecessor);
  }

  while (!q.empty()) {
    auto *predecessor = q.front();
    q.pop();

    if (blocks.find(predecessor) != blocks.end()) {
      continue;
    }

    blocks.insert(predecessor);
    for (auto *transitive : llvm::predecessors(predecessor)) {
      q.push(transitive);
    }
  }

  return blocks;
}

std::set<llvm::Value*> findReachingValues(llvm::Instruction *before) {
  std::set<llvm::Value*> values;
  auto *function = before->getFunction();
  auto *block = before->getParent();
  auto *module = before->getModule();

  // formals
  for (auto &argument : function->args()) {
    values.insert(&argument);
  }

  // globals
  for (auto &global : module->globals()) {
    values.insert(&global);
  }

  // instructions in the same block
  for (auto &instruction : *block) {
    if (&instruction == before) {
      break;
    }
    values.insert(&instruction);
  }

  // find all instructions in transitive predecessor blocks
  // NOTE dominator tree may be needed; this may miss some instructions
  for (auto *predecessor : transitivePredecessors(block)) {
    for (auto &instruction : *predecessor) {
      values.insert(&instruction);
    }
  }

  return values;
}

std::set<llvm::Value*> findReachingValuesWithType(llvm::Instruction *before, llvm::Type *type) {
  std::set<llvm::Value*> output;

  for (auto *value : findReachingValues(before)) {
    if (auto *alloca = llvm::dyn_cast<llvm::AllocaInst>(value)) {
      if (alloca->getAllocatedType() == type) {
        output.insert(value);
      }
    }
    if (auto *globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(value)) {
      if (globalVariable->getType() == type) {
        output.insert(value);
      }
    }
    if (auto *argument = llvm::dyn_cast<llvm::Argument>(value)) {
      if (argument->getType() == type) {
        output.insert(value);
      }
    }
  }

  return output;
}

}
