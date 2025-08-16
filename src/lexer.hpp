#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

#include "token.hpp"

struct Lexer {
    Lexer(const std::string &src);
    ~Lexer();

    // gives the next token
    Token next();
    void retokenize();

    // this is text
    char *text = nullptr;

    size_t idx;
    size_t length;
    size_t line;
    size_t line_start;

  private:
    void trim_left();
    char chop_char();
    bool starts_with(const char *prefix, size_t prefix_len);
};

#endif // LEXER_HPP
