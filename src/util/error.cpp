#include "error.hpp"

Error::Error(Code code, const std::string &message)
    : code(code), message(message) {}

bool Error::ok() const {
    return code == Code::OK;
}

Error::Code Error::output_error() const {
    fmt::println("{}", *this);
    return code;
}
