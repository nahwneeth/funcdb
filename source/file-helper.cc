#include "file-helper.h"

#include <fstream>

namespace funcdb {

namespace stdfs = std::filesystem;

bool CreateFileIfNotExist(stdfs::path path) {
  if (!stdfs::exists(path)) {
    auto file = std::fstream();
    file.open(path, std::ios::out);

    if (file.bad()) return false;
  }
  return true;
}

FileTypeAndPermsResult CheckFileTypeAndPerms(stdfs::path path) {
  auto p = stdfs::status(path).permissions();

  if (!stdfs::is_regular_file(path))
    return FileTypeAndPermsResult::NotRegularFile;

  if ((p & stdfs::perms::owner_read) == stdfs::perms::none)
    return FileTypeAndPermsResult::NoReadPermission;

  if ((p & stdfs::perms::owner_write) == stdfs::perms::none)
    return FileTypeAndPermsResult::NoWritePermission;

  return FileTypeAndPermsResult::Valid;
}

}  // namespace funcdb
