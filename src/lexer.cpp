#include "lexer.hpp"

#include <cstring>

#include "string.hpp"

constexpr static char *keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "for",
    "goto",
    "if",
    "register",
    "return",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "volatile",
    "while",
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "atomic_cancel",
    "atomic_commit",
    "atomic_noexcept",
    "bitand",
    "bitor",
    "bool",
    "catch",
    "class",
    "co_await",
    "co_return",
    "co_yield",
    "compl",
    "concept",
    "const_cast",
    "consteval",
    "constexpr",
    "constinit",
    "decltype",
    "delete",
    "dynamic_cast",
    "explicit",
    "export",
    "friend",
    "inline",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "override",
    "private",
    "protected",
    "public",
    "reflexpr",
    "reinterpret_cast",
    "requires",
    "static_assert",
    "static_cast",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "try",
    "typeid",
    "typename",
    "using",
    "virtual",
    "wchar_t",
    "xor",
    "xor_eq",
    "#include",
    "#ifndef",
    "#ifdef",
    "#endif",
    "#else",
};

constexpr static char *builtin_types[] =
    {"int", "short", "unsigned", "float", "char", "long", "double", "void"};

Lexer::Lexer(const std::string &src)
    : idx(0), length(src.length()), line(0), line_start(0) {
    text = new char[src.length()];
    src.copy(text, src.length());
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

    const char literal_tokens[] = {'(', ')', '{', '}', ';'};
    const TokenType types[] = {TokenType::OPEN_PAREN,
                               TokenType::CLOSE_PAREN,
                               TokenType::OPEN_CURLY,
                               TokenType::CLOSE_CURLY,
                               TokenType::SEMICOLON};
    for (size_t i = 0; i < 5; ++i) {
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

        // find keywords
        for (const auto &kwd : keywords) {
            size_t start_index = token.text - text;
            // if the first chunk is the keyword and they have the same length
            size_t kwd_len = strlen(kwd);
            if (compare(token.text, kwd_len, kwd, kwd_len) &&
                kwd_len == token.len) {
                token.type = TokenType::KEYWORD;
                return token;
            }
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
    const char *ops[] = {"=",
                         "!",
                         "==",
                         ">",
                         "<",
                         ">=",
                         "<=",
                         "!=",
                         "+",
                         "-",
                         "*",
                         "/",
                         "%",
                         "::",
                         "&&",
                         "||"};
    const TokenType token_types[] = {TokenType::ASSIGNMENT,
                                     TokenType::NOT,
                                     TokenType::EQUALS,
                                     TokenType::GT,
                                     TokenType::LT,
                                     TokenType::GOT,
                                     TokenType::LOT,
                                     TokenType::NOT_EQUAL,
                                     TokenType::PLUS,
                                     TokenType::MINUS,
                                     TokenType::MUL,
                                     TokenType::DIV,
                                     TokenType::MOD,
                                     TokenType::SCOPE,
                                     TokenType::AND,
                                     TokenType::OR};
    for (const char *op : ops) {
        size_t i = 0;
        size_t l = strlen(op);
        if (starts_with(op, l)) {
            token.type = token_types[i++];
            for (size_t i = 0; i < l; ++i) {
                chop_char();
            }
            token.len += l;
            return token;
        }
    }

    token.type = TokenType::INVALID;
    token.len = 1;
    chop_char();

    return token;
}
