#ifndef UTIL_FILE_HPP
#define UTIL_FILE_HPP

#include <fstream>
#include <sstream>
#include <string>

std::string load_file(const std::string &file_name) {
    std::ifstream t(file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source = buffer.str();
    return source;
}

#endif // UTIL_FILE_HPP
