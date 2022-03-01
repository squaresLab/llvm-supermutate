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

  FileInfo* get(llvm::DIFile *file);
  FunctionInfo* get(llvm::Function *function);
  InstructionInfo* get(llvm::Instruction *instruction);
  GlobalInfo* get(llvm::GlobalVariable *value);
  VariableInfo* get(llvm::DIVariable *variable);
  ValueInfo* get(llvm::Value *value);

  size_t getID(llvm::DIFile *file);
  size_t getID(llvm::Function *function);
  size_t getID(llvm::Instruction *instruction);
  size_t getID(llvm::GlobalVariable *global);
  size_t getID(llvm::DIVariable *variable);
  size_t getID(llvm::Value *value);

  FileInfo* getFileInfo(size_t id);
  FunctionInfo* getFunctionInfo(size_t id);
  InstructionInfo* getInstructionInfo(size_t id);
  VariableInfo* getVariableInfo(size_t id);
  GlobalInfo* getGlobalInfo(size_t id);
  ValueInfo* getValueInfo(size_t id);

  llvm::DIFile* getFile(size_t id);
  llvm::Function* getFunction(size_t id);
  llvm::Instruction* getInstruction(size_t id);
  llvm::GlobalVariable* getGlobal(size_t id);
  llvm::DIVariable* getVariable(size_t id);
  llvm::Value* getValue(size_t id);

  bool hasInfo(llvm::Instruction *instruction) const;

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
