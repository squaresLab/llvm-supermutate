#include <llvm-supermutate/Mapping/LLVMToSourceMapping.h>

#include <fstream>
#include <iomanip>
#include <ostream>

using json = nlohmann::json;

namespace llvmsupermutate {

LLVMToSourceMapping* LLVMToSourceMapping::build(llvm::Module &module) {
  LLVMToSourceMapping* mapping = new LLVMToSourceMapping();

  for (llvm::GlobalVariable &variable : module.globals()) {
    mapping->getOrCreate(llvm::cast<llvm::Value>(&variable));
  }

  for (llvm::Function &function : module) {
    mapping->build(function);
  }

  return mapping;
}

LLVMToSourceMapping::LLVMToSourceMapping() noexcept
: files(),
  functions(),
  globals(),
  instructions(),
  variables(),
  values(),
  fileToID(),
  functionToID(),
  globalToID(),
  instructionToID(),
  variableToID(),
  valueToID()
{ }

LLVMToSourceMapping::~LLVMToSourceMapping() {
  for (auto *file : files) {
    delete file;
  }
  for (auto *function : functions) {
    delete function;
  }
  for (auto *global : globals) {
    delete global;
  }
  for (auto *instruction : instructions) {
    delete instruction;
  }
  for (auto *variable : variables) {
    delete variable;
  }
  for (auto *value : values) {
    delete value;
  }
}

bool LLVMToSourceMapping::hasInfo(llvm::Instruction *instruction) const {
  return instructionToID.find(instruction) != instructionToID.end();
}

void LLVMToSourceMapping::save(std::string const &filename) const {
  std::ofstream o(filename);
  o << std::setw(2) << toJSON() << std::endl;
}

json LLVMToSourceMapping::toJSON() const {
  return {
    {"files", filesToJSON()},
    {"functions", functionsToJSON()},
    {"globals", globalsToJSON()},
    {"instructions", instructionsToJSON()},
    {"variables", variablesToJSON()},
    {"values", valuesToJSON()}
  };
}

json LLVMToSourceMapping::filesToJSON() const {
  json j = json::array();
  for (auto *fileInfo : files) {
    j.push_back(fileInfo->toJSON());
  }
  return j;
}

json LLVMToSourceMapping::functionsToJSON() const {
  json j = json::array();
  for (auto *functionInfo : functions) {
    j.push_back(functionInfo->toJSON());
  }
  return j;
}

json LLVMToSourceMapping::globalsToJSON() const {
  json j = json::array();
  for (auto *globalInfo : globals) {
    j.push_back(globalInfo->toJSON());
  }
  return j;
}

json LLVMToSourceMapping::valuesToJSON() const {
  json j = json::array();
  for (auto *valueInfo : values) {
    j.push_back(valueInfo->toJSON());
  }
  return j;
}

json LLVMToSourceMapping::instructionsToJSON() const {
  json j = json::array();
  for (auto *instructionInfo : instructions) {
    j.push_back(instructionInfo->toJSON());
  }
  return j;
}

json LLVMToSourceMapping::variablesToJSON() const {
  json j = json::array();
  for (auto *variableInfo : variables) {
    j.push_back(variableInfo->toJSON());
  }
  return j;
}

FileInfo* LLVMToSourceMapping::getFileInfo(size_t id) {
  if (id >= files.size()) {
    return nullptr;
  }
  return files[id];
}

FunctionInfo* LLVMToSourceMapping::getFunctionInfo(size_t id) {
  if (id >= functions.size()) {
    return nullptr;
  }
  return functions[id];
}

InstructionInfo* LLVMToSourceMapping::getInstructionInfo(size_t id) {
  if (id >= instructions.size()) {
    return nullptr;
  }
  return instructions[id];
}

VariableInfo* LLVMToSourceMapping::getVariableInfo(size_t id) {
  if (id >= variables.size()) {
    return nullptr;
  }
  return variables[id];
}

ValueInfo* LLVMToSourceMapping::getValueInfo(size_t id) {
  if (id >= values.size()) {
    return nullptr;
  }
  return values[id];
}

llvm::DIFile* LLVMToSourceMapping::getFile(size_t id) {
  if (auto *info = getFileInfo(id)) {
    return info->getFile();
  } else {
    return nullptr;
  }
}

llvm::Function* LLVMToSourceMapping::getFunction(size_t id) {
  if (auto *info = getFunctionInfo(id)) {
    return info->getFunction();
  } else {
    return nullptr;
  }
}

llvm::Instruction* LLVMToSourceMapping::getInstruction(size_t id) {
  if (auto *info = getInstructionInfo(id)) {
    return info->getInstruction();
  } else {
    return nullptr;
  }
}

llvm::DIVariable* LLVMToSourceMapping::getVariable(size_t id) {
  if (auto *info = getVariableInfo(id)) {
    return info->getVariable();
  } else {
    return nullptr;
  }
}

llvm::Value* LLVMToSourceMapping::getValue(size_t id) {
  if (auto *info = getValueInfo(id)) {
    return info->getValue();
  } else {
    return nullptr;
  }
}

size_t LLVMToSourceMapping::getID(llvm::DIFile *file) {
  return get(file)->getId();
}

size_t LLVMToSourceMapping::getID(llvm::Function *function) {
  return get(function)->getId();
}

size_t LLVMToSourceMapping::getID(llvm::Instruction *instruction) {
  return get(instruction)->getID();
}

size_t LLVMToSourceMapping::getID(llvm::GlobalVariable *global) {
  return get(global)->getID();
}

size_t LLVMToSourceMapping::getID(llvm::DIVariable *variable) {
  return get(variable)->getID();
}

size_t LLVMToSourceMapping::getID(llvm::Value *value) {
  return get(value)->getID();
}

FileInfo* LLVMToSourceMapping::get(llvm::DIFile *file) {
  return files[fileToID[file]];
}

FunctionInfo* LLVMToSourceMapping::get(llvm::Function *function) {
  return functions[functionToID[function]];
}

InstructionInfo* LLVMToSourceMapping::get(llvm::Instruction *instruction) {
  return instructions[instructionToID[instruction]];
}

VariableInfo* LLVMToSourceMapping::get(llvm::DIVariable *variable) {
  return variables[variableToID[variable]];
}

ValueInfo* LLVMToSourceMapping::get(llvm::Value *value) {
  return values[valueToID[value]];
}

void LLVMToSourceMapping::build(llvm::Function &function) {
  assert(functionToID.find(&function) == functionToID.end());

  if (function.isDeclaration() or function.isDeclarationForLinker()) {
    llvm::errs() << "Skipping function declaration: " << function.getName() << "\n";
    return;
  }

  auto *subprogram = function.getSubprogram();
  if (subprogram == nullptr) {
    llvm::errs() << "Skipping function without DI subprogram: " << function.getName() << "\n";
    return;
  }

  auto *file = subprogram->getFile();
  if (file == nullptr) {
    llvm::errs() << "Skipping function without DIFile: " << function.getName() << "\n";
    return;
  }

  // register the formals for this function
  for (llvm::Argument &arg : function.args()) {
    getOrCreate(llvm::cast<llvm::Value>(&arg));
  }

  auto *fileInfo = getOrCreate(file);
  auto *functionInfo = create(&function, fileInfo);
  for (auto &block : function) {
    for (auto &instruction : block) {
      build(instruction, functionInfo);
    }
  }
}

FunctionInfo* LLVMToSourceMapping::create(llvm::Function *function, FileInfo *fileInfo) {
  size_t internal_id = functions.size();
  auto *info = new FunctionInfo(internal_id, fileInfo, function);
  functions.push_back(info);
  functionToID.emplace(function, internal_id);
  return info;
}

FileInfo* LLVMToSourceMapping::getOrCreate(llvm::DIFile *file) {
  auto entry = fileToID.find(file);
  if (entry != fileToID.end()) {
    return files[entry->second];
  }

  size_t internal_id = files.size();
  FileInfo *info = new FileInfo(internal_id, file);
  files.push_back(info);
  fileToID.emplace(file, internal_id);
  return info;
}

void LLVMToSourceMapping::build(llvm::Instruction &instruction, FunctionInfo *functionInfo) {
  create(&instruction, functionInfo);

  // register this instruction and its operands as values
  getOrCreate(llvm::cast<llvm::Value>(&instruction));
  for (auto &operand : instruction.operands()) {
    getOrCreate(llvm::cast<llvm::Value>(&operand));
  }

  if (auto *value = llvm::dyn_cast<llvm::DbgVariableIntrinsic>(&instruction)) {
    if (auto variable = value->getVariable()) {
      getOrCreate(variable, functionInfo);
    }
  }
}

GlobalInfo* LLVMToSourceMapping::create(llvm::GlobalVariable *variable) {
  // use this opportunity to register the underlying llvm::Value
  getOrCreate(llvm::cast<llvm::Value>(variable));

  size_t internal_id = globals.size();
  auto *info = new GlobalInfo(internal_id, variable);
  globals.push_back(info);
  globalToID.emplace(variable, internal_id);
  return info;
}

InstructionInfo* LLVMToSourceMapping::create(llvm::Instruction *instruction, FunctionInfo *functionInfo) {
  size_t internal_id = instructions.size();
  auto *info = new InstructionInfo(internal_id, instruction, functionInfo);
  instructions.push_back(info);
  instructionToID.emplace(instruction, internal_id);
  return info;
}

ValueInfo* LLVMToSourceMapping::getOrCreate(llvm::Value *value) {
  auto entry = valueToID.find(value);
  if (entry != valueToID.end()) {
    return values[entry->second];
  }

  size_t internal_id = values.size();
  auto *info = new ValueInfo(internal_id, value);
  values.push_back(info);
  valueToID.emplace(value, internal_id);
  return info;
}

VariableInfo* LLVMToSourceMapping::getOrCreate(llvm::DIVariable *variable, FunctionInfo *functionInfo) {
  auto entry = variableToID.find(variable);
  if (entry != variableToID.end()) {
    return variables[entry->second];
  }

  size_t internal_id = variables.size();
  auto *info = new VariableInfo(internal_id, variable, functionInfo);
  variables.push_back(info);
  variableToID.emplace(variable, internal_id);
  return info;
}

void LLVMToSourceMapping::dump() {
  llvm::outs() << "GLOBALS:\n";
  for (auto *global : globals) {
    llvm::outs() << "G" << global->getID() << ": " << global->getName() << "\n";
  }

  llvm::outs() << "FILES:\n";
  for (auto *file : files) {
    llvm::outs() << "F" << file->getId() << ": " << file->getFilename() << "\n";
  }

  llvm::outs() << "\nFUNCTIONS:\n";
  for (auto *function : functions) {
    llvm::outs() << "f" << function->getId() << ": " << function->getName() << "\n";
  }

  llvm::outs() << "\nINSTRUCTIONS:\n";
  for (auto *instruction : instructions) {
    llvm::outs() << "i" << instruction->getID() << ": " << instruction->getFilename() << "@";

    auto maybeLineCol = instruction->getLineCol();
    if (maybeLineCol.hasValue()) {
      auto lineCol = maybeLineCol.getValue();
      llvm::outs() << lineCol.first << ":" << lineCol.second;
    } else {
      llvm::outs() << "implicit";
    }
    llvm::outs() << "\n";
  }

  llvm::outs() << "\nVARIABLES:\n";
  for (auto *variable : variables) {
    llvm::outs() << "v" << variable->getID() << ": " << variable->getName() << "\n";
  }
}

} // llvmsupermutate
