#ifndef UTIL_FILE_HPP
#define UTIL_FILE_HPP

#include <string>

#include "util/status.hpp"

StatusOr<std::string> load_file(const std::string &file_name);

#endif // UTIL_FILE_HPP
