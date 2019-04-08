#ifndef CS184_FILE_UTILS_H
#define CS184_FILE_UTILS_H

#include <set>
#include <string>

namespace FileUtils {

bool list_files_in_directory(const std::string& dir_path, std::set<std::string>& retval);
bool split_filename(const std::string& filename, std::string& before_extension, std::string& extension);
bool file_exists(const std::string& filename);

}


#endif // CS184_FILE_UTILS_H
