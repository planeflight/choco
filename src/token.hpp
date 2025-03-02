#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    END = 0,
    INVALID,
    SYMBOL,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,
    SEMICOLON,
    KEYWORD,
    TYPE,
    NUMBER,
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
    OR
};

struct Token {
    TokenType type;
    const char *text;
    size_t len = 0;

    std::string to_string();
};
#endif // TOKEN_HPP
