#ifndef UTIL_ERROR_HPP
#define UTIL_ERROR_HPP

#include <fmt/core.h>

#include <string>

#include "util/util.hpp"

class Error {
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
    Error(Code code, const std::string &message = "");

    bool ok() const;

    Code output_error() const;

    Code error() const {
        return code;
    }

  private:
    Code code;
    std::string message;
    friend struct fmt::formatter<Error>;
};

template <>
struct fmt::formatter<Error::Code> : fmt::formatter<std::string> {
    auto format(Error::Code c, fmt::format_context &ctx) const {
        std::string code_str;
        switch (c) {
            case Error::Code::OK:
                code_str = "OK";
                break;
            case Error::Code::INVALID_ARGUMENT_ERROR:
                code_str = "Invalid Argument Error";
                break;
            case Error::Code::SYNTAX_ERROR:
                code_str = "Syntax Error";
                break;
            case Error::Code::INDEX_ERROR:
                code_str = "Index Error";
                break;
            case Error::Code::ARGUMENT_ERROR:
                code_str = "Argument Error";
                break;
            case Error::Code::NAME_ERROR:
                code_str = "Name Error";
                break;
            case Error::Code::ZERO_DIVISION_ERROR:
                code_str = "Zero Division Error";
                break;
            case Error::Code::TYPE_ERROR:
                code_str = "Type Error";
                break;
            case Error::Code::FILE_NOT_FOUND_ERROR:
                code_str = "File Not Found Error";
                break;
        }
        // use fmt inside to build a string
        return fmt::formatter<std::string>::format(fmt::format("{}", code_str),
                                                   ctx);
    }
};

template <>
struct fmt::formatter<Error> : fmt::formatter<std::string> {
    auto format(Error s, fmt::format_context &ctx) const {
        // use fmt inside to build a string
        return fmt::formatter<std::string>::format(
            fmt::format("{}: {}", s.code, s.message), ctx);
    }
};

#endif // UTIL_ERROR_HPP
