#include <llvm-supermutate/Mapping/File.h>

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

json FileInfo::toJSON() const {
  return {
    {"id", id},
    {"filename", getFilename()},
  };
}

} // llvmsupermutate
