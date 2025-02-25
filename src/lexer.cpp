#include "lexer.hpp"

Lexer::Lexer(const std::string &src) : idx(0), line(0), line_start(0) {
    src.copy(text, src.length());
}
