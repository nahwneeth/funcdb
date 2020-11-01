#if !defined(FUNCDB_FILE_HELPER_H)
#define FUNCDB_FILE_HELPER_H

#include <filesystem>

namespace funcdb {

bool CreateFileIfNotExist(std::filesystem::path path);

enum class FileTypeAndPermsResult {
  NoReadPermission,
  NotRegularFile,
  NoWritePermission,
  Valid
};

FileTypeAndPermsResult CheckFileTypeAndPerms(std::filesystem::path path);

}  // namespace funcdb

#endif  // FUNCDB_FILE_HELPER_H
