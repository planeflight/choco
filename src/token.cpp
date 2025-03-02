#include "token.hpp"

std::string Token::to_string() {
    switch (type) {
        case TokenType::END:
            return "End of Content";
        case TokenType::INVALID:
            return "Invalid";
        case TokenType::SYMBOL:
            return "Symbol";
        case TokenType::OPEN_PAREN:
            return "Open parentheses";
        case TokenType::CLOSE_PAREN:
            return "Close paratheses";
        case TokenType::OPEN_CURLY:
            return "Open curly";
        case TokenType::CLOSE_CURLY:
            return "Close curly";
        case TokenType::SEMICOLON:
            return "Semicolon";
        case TokenType::KEYWORD:
            return "Keyword";
        case TokenType::TYPE:
            return "Type";
        case TokenType::NUMBER:
            return "Number";
        case TokenType::STRING:
            return "String";
    }
    return "";
}
