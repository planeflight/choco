#include "token.hpp"

#include <sstream>
#include <string>

std::string type_to_string(TokenType type) {
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

        case TokenType::USE:
            return "Use";
        case TokenType::LET:
            return "Let";
        case TokenType::IF:
            return "If";
        case TokenType::ELSE:
            return "Else";
        case TokenType::WHILE:
            return "While";
        case TokenType::FOR:
            return "For";
        case TokenType::RETURN:
            return "Return";
        case TokenType::CLASS:
            return "Class";

        case TokenType::TYPE:
            return "Type";
        case TokenType::NUMBER:
            return "Number";
        case TokenType::ASSIGNMENT:
            return "Assignment";
        case TokenType::PLUS:
            return "Plus";
        case TokenType::MUL:
            return "Multiply";
        case TokenType::STRING:
            return "String";
        case TokenType::COMMA:
            return "Comma";
        case TokenType::BOOLEAN:
            return "Boolean";
        case TokenType::LT:
            return "Less Than";
        case TokenType::GT:
            return "Greater Than";
        case TokenType::OPEN_BRACKET:
            return "[";
        case TokenType::CLOSE_BRACKET:
            return "]";
    }
    return "";
}

std::string Token::to_string() {
    return type_to_string(type);
}

std::string Token::content() {
    std::stringstream ss;
    for (int i = 0; i < len; ++i) ss << text[i];
    return ss.str();
}
