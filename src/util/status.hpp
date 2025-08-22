#ifndef UTIL_STATUS_HPP
#define UTIL_STATUS_HPP

#include <fmt/core.h>

#include <string>

#include "util/util.hpp"

class Status {
  public:
    enum Code : int {
        OK = 0,
        INVALID_ARGUMENT_ERROR,
        SYNTAX_ERROR,
        INDEX_ERROR,
        ARGUMENT_ERROR,
        NAME_ERROR,
        ZERO_DIVISION_ERROR,
        TYPE_ERROR,
        FILE_NOT_FOUND_ERROR,
    };
    Status(Code code, const std::string &message = "");

    bool ok() const;

    void output_error() const;

    Code error() const {
        return code;
    }

  private:
    Code code;
    std::string message;
    friend struct fmt::formatter<Status>;
};

template <typename T>
class StatusOr : public Status {
  public:
    StatusOr(const T &value) : Status(Code::OK), object(std::move(value)) {}
    StatusOr(T &&value) : Status(Code::OK), object(std::move(value)) {}
    StatusOr(T &value) : Status(Code::OK), object(std::move(value)) {}

    StatusOr(Code code, const std::string &message = "")
        : Status(code, message) {}

    T &operator*() {
        return object;
    }
    const T &operator*() const {
        return object;
    }

    T &operator->() {
        return object;
    }
    const T &operator->() const {
        return object;
    }

  private:
    T object;
};

template <>
struct fmt::formatter<Status::Code> : fmt::formatter<std::string> {
    auto format(Status::Code c, fmt::format_context &ctx) const {
        std::string code_str;
        switch (c) {
            case Status::Code::OK:
                code_str = "OK";
                break;
            case Status::Code::INVALID_ARGUMENT_ERROR:
                code_str = "Invalid Argument Error";
                break;
            case Status::Code::SYNTAX_ERROR:
                code_str = "Syntax Error";
                break;
            case Status::Code::INDEX_ERROR:
                code_str = "Index Error";
                break;
            case Status::Code::ARGUMENT_ERROR:
                code_str = "Argument Error";
                break;
            case Status::Code::NAME_ERROR:
                code_str = "Name Error";
                break;
            case Status::Code::ZERO_DIVISION_ERROR:
                code_str = "Zero Division Error";
                break;
            case Status::Code::TYPE_ERROR:
                code_str = "Type Error";
                break;
            case Status::Code::FILE_NOT_FOUND_ERROR:
                code_str = "File Not Found Error";
                break;
        }
        // use fmt inside to build a string
        return fmt::formatter<std::string>::format(fmt::format("{}", code_str),
                                                   ctx);
    }
};

template <>
struct fmt::formatter<Status> : fmt::formatter<std::string> {
    auto format(Status s, fmt::format_context &ctx) const {
        // use fmt inside to build a string
        return fmt::formatter<std::string>::format(
            fmt::format("{}: {}", s.code, s.message), ctx);
    }
};

#endif // UTIL_STATUS_HPP
