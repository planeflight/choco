#include "lexer.hpp"

#include <cstring>
#include <iostream>

#include "token.hpp"
#include "util/string.hpp"

constexpr static char *builtin_types[] =
    {"int", "short", "unsigned", "float", "char", "long", "double", "void"};

Lexer::Lexer(const std::string &src)
    : idx(0), length(src.length()), line(0), line_start(0) {
    text = new char[src.length()];
    src.copy(text, src.length());
}

Lexer::~Lexer() {
    delete[] text;
}

void Lexer::retokenize() {
    idx = 0;
    line = 0;
    line_start = 0;
}

void Lexer::trim_left() {
    while (idx < length && isspace(text[idx])) {
        chop_char();
    }
}

bool is_symbol_start(char x) {
    // can't start with a num
    return isalpha(x) || x == '_';
}

bool is_symbol(char x) {
    // can be a num
    return isalnum(x) || x == '_';
}

bool is_num(char x) {
    return isdigit(x) || x == '.' || x == 'f' || x == 'u' || x == 'b' ||
           x == 'x';
}

bool is_num_start(char x) {
    return isdigit(x);
}

char Lexer::chop_char() {
    char x = text[idx];
    idx++;
    if (x == '\n') {
        line++;
        line_start = idx;
    } else {
    }
    return x;
}

bool Lexer::starts_with(const char *prefix, size_t prefix_len) {
    if (prefix_len == 0) {
        return true;
    }
    if (idx + prefix_len - 1 >= length) {
        return false;
    }

    // check if the prefix is equal to the current idx text
    for (size_t i = 0; i < prefix_len; ++i) {
        if (prefix[i] != text[idx + i]) {
            return false;
        }
    }
    return true;
}

Token Lexer::next() {
    trim_left();
    Token token{TokenType::END, &text[idx], 0};
    if (idx >= length) return token;

    size_t len = length;

    const char literal_tokens[] = {'(', ')', '{', '}', ';', ','};
    const TokenType types[] = {TokenType::OPEN_PAREN,
                               TokenType::CLOSE_PAREN,
                               TokenType::OPEN_CURLY,
                               TokenType::CLOSE_CURLY,
                               TokenType::SEMICOLON,
                               TokenType::COMMA};
    for (size_t i = 0; i < 6; ++i) {
        char c = literal_tokens[i];
        if (c == text[idx]) {
            token.type = types[i];
            token.len++;
            chop_char();
            return token;
        }
    }

    // strings
    if (text[idx] == '"') {
        token.type = TokenType::STRING;
        // get first piece
        chop_char();
        token.len++;
        while (idx < len && text[idx] != '"') {
            chop_char();
            token.len++;
        }
        // get last "
        if (idx < len) {
            chop_char();
            token.len++;
        }
        return token;
    }

    // all symbols
    if (is_symbol_start(text[idx])) {
        token.type = TokenType::SYMBOL;
        while (idx < len && is_symbol(text[idx])) {
            chop_char();
            token.len++;
        }

        const TokenType type[] = {TokenType::USE,
                                  TokenType::LET,
                                  TokenType::IF,
                                  TokenType::ELIF,
                                  TokenType::ELSE,
                                  TokenType::WHILE,
                                  TokenType::FOR,
                                  TokenType::FUNCTION,
                                  TokenType::RETURN,
                                  TokenType::BOOLEAN,
                                  TokenType::BOOLEAN};

        const char *keywords[] = {
            "use",
            "let",
            "if",
            "elif",
            "else",
            "while",
            "for",
            "confection", // TODO: config file to change keywords
            "gift",
            "true",
            "false"};

        // find keywords
        size_t i = 0;
        for (const auto &kwd : keywords) {
            size_t start_index = token.text - text;
            // if the first chunk is the keyword and they have the same length
            size_t kwd_len = strlen(kwd);
            if (compare(token.text, kwd_len, kwd, kwd_len) &&
                kwd_len == token.len) {
                token.type = type[i];
                return token;
            }
            i++;
        }

        // find types
        for (const auto &type : builtin_types) {
            size_t start_index = token.text - text;
            // if the first chunk is the keyword and they have the same length
            size_t type_len = strlen(type);
            if (compare(token.text, type_len, type, type_len) &&
                type_len == token.len) {
                token.type = TokenType::TYPE;
                return token;
            }
        }
        return token;
    }
    // find numbers
    if (is_num_start(text[idx])) {
        token.type = TokenType::NUMBER;
        while (idx < len && is_num(text[idx])) {
            chop_char();
            token.len++;
        }
        return token;
    }

    // find comments
    if (starts_with("//", 2)) {
        token.type = TokenType::COMMENT;
        while (idx < len && text[idx] != '\n') {
            chop_char();
            token.len++;
        }
        return token;
    }

    // find some operators
    // WARN: order of == vs = and >= vs >, etc important because it finds this
    // first so it marks this as the correct operator rather than '=' and
    // something else
    const char *ops[] = {"==",
                         "!=",
                         "=",
                         ">=",
                         "<=",
                         ">",
                         "<",
                         "!",
                         "+",
                         "-",
                         "*",
                         "/",
                         "%",
                         "::",
                         "&&",
                         "||"};
    const TokenType token_types[] = {TokenType::EQUALS,
                                     TokenType::NOT_EQUAL,
                                     TokenType::ASSIGNMENT,
                                     TokenType::GOT,
                                     TokenType::LOT,
                                     TokenType::GT,
                                     TokenType::LT,
                                     TokenType::NOT,
                                     TokenType::PLUS,
                                     TokenType::MINUS,
                                     TokenType::MUL,
                                     TokenType::DIV,
                                     TokenType::MOD,
                                     TokenType::SCOPE,
                                     TokenType::AND,
                                     TokenType::OR};
    size_t j = 0;
    for (const char *op : ops) {
        size_t l = strlen(op);
        if (starts_with(op, l)) {
            token.type = token_types[j];
            for (size_t i = 0; i < l; ++i) {
                chop_char();
            }
            token.len += l;
            return token;
        }
        j++;
    }

    token.type = TokenType::INVALID;
    token.len = 1;
    chop_char();

    return token;
}
