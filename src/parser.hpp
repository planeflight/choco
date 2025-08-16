#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>
#include <memory>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

class Parser {
  public:
    Parser(const std::vector<Token> &tokens);
    ~Parser();

    const std::vector<uptr<Statement>> &parse();

    Token *expect(TokenType type);
    bool match(TokenType type);
    bool match_peek(TokenType type);

    // token traversing
    Token *peek();
    Token *current();
    Token *previous();
    void advance();

    uptr<Expr> parse_expression();
    uptr<ASTNode> parse_statement();

    uptr<Expr> expression();
    uptr<Expr> equality();
    uptr<Expr> comparison();
    uptr<Expr> term();
    uptr<Expr> factor();
    uptr<Expr> unary();
    uptr<Expr> primary();

  private:
    std::vector<Token> tokens;
    size_t idx = 0;

    std::vector<uptr<Statement>> statements;
};

#endif // PARSER_HPP
