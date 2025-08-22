#include "status.hpp"

Status::Status(Code code, const std::string &message)
    : code(code), message(message) {}

bool Status::ok() const {
    return code == Code::OK;
}
