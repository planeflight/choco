#ifndef RUNTIME_INTERPETER_HPP
#define RUNTIME_INTERPETER_HPP

#include "ast.hpp"
#include "runtime/memory.hpp"
#include "runtime/runtime.hpp"
#include "runtime/value.hpp"
#include "token.hpp"
#include "util/util.hpp"

struct Interpreter {
    Interpreter();
    LiteralValue *evaluate(Statement *statement);
    LiteralValue *evaluate_variable_declaration(VariableDeclaration *v);

    LiteralValue *evaluate_function_call(CallExpr *s);
    LiteralValue *evaluate_if_statement(IfExpr *s);
    LiteralValue *evaluate_while_statement(WhileExpr *s);

    LiteralValue *evaluate_expr(Expr *expr);
    LiteralValue *evaluate_binary_expr(BinaryExpr *v);
    LiteralValue *evaluate_unary_expr(UnaryExpr *v);

    // INFO: predefined STDIO functions
    LiteralValue *print(CallExpr *s);
    LiteralValue *input(CallExpr *s);

  private:
    Runtime global_runtime;
    Memory memory;
};

#endif // RUNTIME_INTERPETER_HPP
