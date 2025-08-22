#include "file.hpp"

#include <fmt/core.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "util/error.hpp"

std::string load_file(const std::string &file_name) {
    std::ifstream t(file_name);
    if (t) {
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string source = buffer.str();
        return source;
    }
    throw Error(Error::Code::FILE_NOT_FOUND_ERROR,
                fmt::format("File '{}' could not be found.", file_name));
}
