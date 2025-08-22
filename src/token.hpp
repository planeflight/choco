#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <fmt/core.h>

#include <string>

enum class TokenType {
    END = 0,
    INVALID,
    SYMBOL,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    SEMICOLON,
    KEYWORD,
    TYPE,

    USE,
    LET,
    IF,
    ELIF,
    ELSE,
    WHILE,
    FOR,
    FUNCTION,
    VARIABLE,
    RETURN,
    CLASS,
    NEW,

    NUMBER,
    BOOLEAN,
    STRING,
    COMMENT,
    ASSIGNMENT,
    NOT,
    EQUALS,
    GT,
    LT,
    GOT,
    LOT,
    NOT_EQUAL,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    SCOPE,
    AND,
    OR,
    COMMA,
    DOT
};

std::string type_to_string(TokenType type);

template <>
struct fmt::formatter<TokenType> : fmt::formatter<std::string> {
    auto format(TokenType t, fmt::format_context &ctx) const {
        // use fmt inside to build a string
        return fmt::formatter<std::string>::format(
            fmt::format("{}", type_to_string(t)), ctx);
    }
};

struct Token {
    TokenType type;
    const char *text;
    size_t len = 0;

    std::string to_string();
    std::string content();
};
#endif // TOKEN_HPP
