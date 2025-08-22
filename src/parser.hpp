#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>
#include <memory>
#include <vector>

#include "ast.hpp"
#include "token.hpp"
#include "util/status.hpp"

class Parser {
  public:
    Parser(const std::vector<Token> &tokens);
    ~Parser();

    const std::vector<uptr<Statement>> &parse();

  private:
    StatusOr<Token *> expect(TokenType type);
    bool match(TokenType type);
    bool match_peek(TokenType type);

    // token traversing
    Token *peek();
    Token *current();
    Token *previous();
    void advance();

    std::vector<uptr<Statement>> parse_body();

    uptr<Expr> declaration();
    uptr<Expr> statement();
    uptr<Expr> expression();
    uptr<Expr> or_expr();
    uptr<Expr> and_expr();
    uptr<Expr> equality();
    uptr<Expr> comparison();
    uptr<Expr> term();
    uptr<Expr> factor();
    uptr<Expr> unary();
    uptr<Expr> postfix();
    uptr<Expr> primary();

    uptr<VariableDeclaration> var_declaration();
    uptr<Expr> finish_call(uptr<SymbolExpr> expr);
    uptr<FunctionDefExpr> function_def();
    uptr<Expr> class_def();

    std::vector<Token> tokens;
    size_t idx = 0;

    std::vector<uptr<Statement>> statements;
};

#endif // PARSER_HPP
