#include <llvm-supermutate/Mapping/File.h>

#include <llvm-supermutate/Utils.h>

using json = nlohmann::json;

namespace llvmsupermutate {

FileInfo::FileInfo(size_t id, llvm::DIFile *file)
  : id(id), file(file)
{ }

size_t FileInfo::getId() const {
  return id;
}

llvm::DIFile* FileInfo::getFile() const {
  return file;
}

std::string FileInfo::getFilename() const {
  return file->getFilename().str();
}

std::string FileInfo::getDirectory() const {
  return file->getDirectory().str();
}

std::string FileInfo::getPath() const {
  return getLlvmFileAbsPath(file);
}

json FileInfo::toJSON() const {
  return {
    {"id", id},
    {"filename", getFilename()},
    {"directory", getDirectory()},
    {"path", getPath()}
  };
}

} // llvmsupermutate
