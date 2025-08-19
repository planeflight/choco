#ifndef RUNTIME_INTERPETER_HPP
#define RUNTIME_INTERPETER_HPP

#include "ast.hpp"
#include "runtime/memory.hpp"
#include "runtime/runtime.hpp"
#include "runtime/scope.hpp"
#include "runtime/value.hpp"
#include "token.hpp"
#include "util/util.hpp"

struct Interpreter {
    Interpreter();
    void eval(const std::vector<uptr<Statement>> &ast);
    LiteralValue *evaluate(Statement *statement, Scope *scope);
    LiteralValue *evaluate_variable_declaration(VariableDeclaration *v,
                                                Scope *scope);

    LiteralValue *evaluate_function_call(CallExpr *s, Scope *scope);
    LiteralValue *evaluate_function_definition(FunctionDefExpr *s,
                                               Scope *scope);
    LiteralValue *evaluate_if_statement(IfExpr *s, Scope *scope);
    LiteralValue *evaluate_while_statement(WhileExpr *s, Scope *scope);
    LiteralValue *evaluate_return_statement(ReturnExpr *s, Scope *scope);

    LiteralValue *evaluate_expr(Expr *expr, Scope *scope);
    LiteralValue *evaluate_binary_expr(BinaryExpr *v, Scope *scope);
    LiteralValue *evaluate_unary_expr(UnaryExpr *v, Scope *scope);

    // INFO: predefined STDIO functions
    LiteralValue *print(CallExpr *s, Scope *scope);
    LiteralValue *input(CallExpr *s, Scope *scope);

  private:
    Scope global_scope;
    Memory memory;
};

#endif // RUNTIME_INTERPETER_HPP
