#pragma once

#include <unordered_map>
#include <vector>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>

#include <nlohmann/json.hpp>

#include "File.h"
#include "Function.h"
#include "Global.h"
#include "Instruction.h"
#include "Value.h"
#include "Variable.h"

namespace llvmsupermutate {

class LLVMToSourceMapping {
public:
  static LLVMToSourceMapping* build(llvm::Module &module);

  LLVMToSourceMapping() noexcept;
  ~LLVMToSourceMapping();

  FileInfo* get(llvm::DIFile *file) const;
  FunctionInfo* get(llvm::Function *function) const;
  InstructionInfo* get(llvm::Instruction *instruction) const;
  GlobalInfo* get(llvm::GlobalVariable *value) const;
  VariableInfo* get(llvm::DIVariable *variable) const;
  ValueInfo* get(llvm::Value *value) const;

  size_t getID(llvm::DIFile *file) const;
  size_t getID(llvm::Function *function) const;
  size_t getID(llvm::Instruction *instruction) const;
  size_t getID(llvm::GlobalVariable *global) const;
  size_t getID(llvm::DIVariable *variable) const;
  size_t getID(llvm::Value *value) const;

  FileInfo* getFileInfo(size_t id) const;
  FunctionInfo* getFunctionInfo(size_t id) const;
  InstructionInfo* getInstructionInfo(size_t id) const;
  VariableInfo* getVariableInfo(size_t id) const;
  GlobalInfo* getGlobalInfo(size_t id) const;
  ValueInfo* getValueInfo(size_t id) const;

  llvm::DIFile* getFile(size_t id) const;
  llvm::Function* getFunction(size_t id) const;
  llvm::Instruction* getInstruction(size_t id) const;
  llvm::GlobalVariable* getGlobal(size_t id) const;
  llvm::DIVariable* getVariable(size_t id) const;
  llvm::Value* getValue(size_t id) const;

  void dump();

  /** Saves the contents of this source mapping to disk */
  void save(std::string const &filename) const;

  nlohmann::json toJSON() const;

private:
  // TODO use unique_ptr
  std::vector<FileInfo*> files;
  std::vector<FunctionInfo*> functions;
  std::vector<GlobalInfo*> globals;
  std::vector<InstructionInfo*> instructions;
  std::vector<VariableInfo*> variables;
  std::vector<ValueInfo*> values;

  std::unordered_map<llvm::DIFile*, size_t> fileToID;
  std::unordered_map<llvm::Function*, size_t> functionToID;
  std::unordered_map<llvm::GlobalVariable*, size_t> globalToID;
  std::unordered_map<llvm::Instruction*, size_t> instructionToID;
  std::unordered_map<llvm::DIVariable*, size_t> variableToID;
  std::unordered_map<llvm::Value*, size_t> valueToID;

  void build(llvm::Function &function);
  void build(llvm::Instruction &instruction, FunctionInfo *functionInfo);

  GlobalInfo* create(llvm::GlobalVariable *variable);
  FunctionInfo* create(llvm::Function *function, FileInfo *fileInfo);
  InstructionInfo* create(llvm::Instruction *instruction, FunctionInfo *functionInfo);

  FileInfo* getOrCreate(llvm::DIFile *file);
  ValueInfo* getOrCreate(llvm::Value *value);
  VariableInfo* getOrCreate(llvm::DIVariable *variable, FunctionInfo *functionInfo);

  nlohmann::json filesToJSON() const;
  nlohmann::json functionsToJSON() const;
  nlohmann::json instructionsToJSON() const;
  nlohmann::json variablesToJSON() const;
  nlohmann::json globalsToJSON() const;
  nlohmann::json valuesToJSON() const;

}; // LLVMToSourceMapping

} // llvmsupermutate
