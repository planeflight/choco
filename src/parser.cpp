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

Token *Parser::previous() {
    return &tokens[idx - 1];
}

void Parser::advance() {
    idx++;
}

uptr<ASTNode> Parser::parse_statement() {
    if (match(TokenType::PRINT)) {
        advance();
        uptr<CallExpr> print = std::make_unique<CallExpr>("print");
        expect(TokenType::OPEN_PAREN);
        // add the parameters
        print->params.push_back(expression());

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

        declaration->value = expression();
        expect(TokenType::SEMICOLON);
        return declaration;
    }
    UNIMPLEMENTED();
    return nullptr;
}

uptr<Expr> Parser::expression() {
    return equality();
}

uptr<Expr> Parser::equality() {
    auto comp =
        comparison(); // the recursive descent part if it's not an equality

    while (match(TokenType::NOT_EQUAL) || match(TokenType::EQUALS)) {
        advance();
        Token *op = previous();
        auto right = comparison();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(comp);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        return binary_expr;
    }
    return comp;
}

uptr<Expr> Parser::comparison() {
    auto left = term();
    while (match(TokenType::LT) || match(TokenType::GT) ||
           match(TokenType::LOT) || match(TokenType::GOT)) {
        advance();
        Token *op = previous();
        auto right = term();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::term() {
    auto left = factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        advance();
        Token *op = previous();
        auto right = factor();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::factor() {
    auto left = unary();
    while (match(TokenType::MUL) || match(TokenType::DIV)) {
        advance();
        Token *op = previous();
        auto right = unary();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::unary() {
    // (! | - ) unary | primary
    while (match(TokenType::NOT) || match(TokenType::MINUS)) {
        advance();
        Token *op = previous();
        auto unary_expr = std::make_unique<UnaryExpr>();
        unary_expr->op = op->type;
        unary_expr->unary = unary(); // recursive unary
        return unary_expr;
    }
    // must be primary
    return primary();
}

uptr<Expr> Parser::primary() {
    // lowest precendence: "last step in parsing"
    // i.e. literals are already evaluated, parentheses are first to be
    // evaluated
    const std::string &curr_content = current()->content();
    if (match(TokenType::STRING)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        // trim the quotes at the beginning and end
        expr->value = std::make_unique<StringValue>(
            curr_content.substr(1, curr_content.size() - 2));
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
    if (match(TokenType::SYMBOL)) {
        uptr<SymbolExpr> symbol = std::make_unique<SymbolExpr>(curr_content);
        advance();
        return symbol;
    }
    // parentheses
    if (match(TokenType::OPEN_PAREN)) {
        advance();
        auto expr = expression();
        expect(TokenType::CLOSE_PAREN);
        return expr;
    }
    return nullptr;
}
