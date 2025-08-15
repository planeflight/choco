#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <string>

#include "ast.hpp"
#include "runtime/value.hpp"
#include "token.hpp"
#include "util/util.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

Parser::~Parser() {}

Token *Parser::expect(TokenType type) {
    if (!match(type))
        throw std::runtime_error("Error: expected " + type_to_string(type) +
                                 " but got " + type_to_string(current()->type) +
                                 "\n");
    Token *curr = current();
    advance();
    return curr;
}

bool Parser::match(TokenType type) {
    return current()->type == type;
}

bool Parser::match_peek(TokenType type) {
    return peek()->type == type;
}

const std::vector<uptr<Statement>> &Parser::parse() {
    while (current()->type != TokenType::END) {
        uptr<Statement> s = parse_statement();
        statements.push_back(std::move(s));
    }
    return statements;
}

Token *Parser::peek() {
    return &tokens[idx + 1];
}

Token *Parser::current() {
    return &tokens[idx];
}

void Parser::advance() {
    idx++;
}

uptr<Expr> Parser::parse_expression() {
    const std::string &curr_content = current()->content();
    if (match(TokenType::STRING)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        expr->value = std::make_unique<StringValue>(curr_content);
        advance();
        return expr;
    }
    if (match(TokenType::MINUS) && match_peek(TokenType::NUMBER)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        // advance the minus sign
        advance();
        expr->value =
            std::make_unique<NumValue>(-std::stod(current()->content()));
        advance();

        return expr;
    }
    if (match(TokenType::NUMBER)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        expr->value = std::make_unique<NumValue>(std::stod(curr_content));
        advance();
        return expr;
    }
    if (match(TokenType::BOOLEAN)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        expr->value = std::make_unique<BoolValue>(curr_content == "true");
        advance();
        return expr;
    }
    if (match(TokenType::SYMBOL) && match_peek(TokenType::CLOSE_PAREN)) {
        uptr<SymbolExpr> symbol = std::make_unique<SymbolExpr>(curr_content);
        advance();
        return symbol;
    }
    UNIMPLEMENTED();
    return nullptr;
}

uptr<ASTNode> Parser::parse_statement() {
    if (match(TokenType::PRINT)) {
        advance();
        uptr<CallExpr> print = std::make_unique<CallExpr>("print");
        expect(TokenType::OPEN_PAREN);
        // add the parameters
        print->params.push_back(parse_expression());

        expect(TokenType::CLOSE_PAREN);
        expect(TokenType::SEMICOLON);
        return print;
    }
    if (match(TokenType::LET)) {
        advance();
        Token *symbol = expect(TokenType::SYMBOL);
        uptr<VariableDeclaration> declaration =
            std::make_unique<VariableDeclaration>();
        declaration->name = symbol->content();
        expect(TokenType::ASSIGNMENT);

        declaration->value = parse_expression();
        expect(TokenType::SEMICOLON);
        return declaration;
    }
    UNIMPLEMENTED();
    return nullptr;
}
