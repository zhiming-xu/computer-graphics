#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif // WIN32

#include <fstream>

#include "file_utils.h"

namespace FileUtils {

bool list_files_in_directory(const std::string& dir_path, std::set<std::string>& retval) {
  // Open directory
  DIR* de_dir = opendir(dir_path.c_str());
  
  // Check we opened correctly
  if (de_dir == NULL) {
    return false;
  }
  
  // Gather all files contained within
  retval.clear();
  for (dirent* de_ent = readdir(de_dir); de_ent != NULL; de_ent = readdir(de_dir)) {
    std::string fname = std::string(de_ent->d_name);
    if (fname == ".." || fname == ".") {
      continue;
    }
    retval.insert(fname);
  }
  
  // Don't forget to close
  closedir(de_dir);
  
  // Success
  return true;
}

bool split_filename(const std::string& filename, std::string& before_extension, std::string& extension) {
  std::size_t dot_pos = filename.find_last_of('.');
  if (dot_pos == std::string::npos) {
    before_extension = filename;
    extension = "";
    return false;
  } else {
    before_extension = filename.substr(0, dot_pos);
    extension = filename.substr(dot_pos + 1);
    return true;
  }
}



bool file_exists(const std::string& test_filename) {
  std::ifstream temp(test_filename);
  if (temp.fail()) {
    return false;
  }
  temp.close();
  return true;
}

}
