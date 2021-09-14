#pragma once

#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include <nlohmann/json.hpp>

namespace llvmsupermutate {

class FileInfo {
public:
  FileInfo(size_t id, llvm::DIFile *file);

  size_t getId() const;
  llvm::DIFile* getFile() const;
  std::string getFilename() const;

  nlohmann::json toJSON() const;

private:
  size_t id;
  llvm::DIFile *file;

}; // FileInfo

} // llvmsupermutate
