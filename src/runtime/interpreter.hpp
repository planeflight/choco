#ifndef RUNTIME_INTERPETER_HPP
#define RUNTIME_INTERPETER_HPP

#include "ast.hpp"
#include "runtime/memory.hpp"
#include "runtime/runtime.hpp"
#include "runtime/scope.hpp"
#include "runtime/value.hpp"
#include "token.hpp"
#include "util/error.hpp"
#include "util/util.hpp"

class Interpreter {
  public:
    Interpreter();
    void eval(const std::vector<uptr<Statement>> &ast);

    // open for testing
    LiteralValue *evaluate(Statement *statement, Scope *scope);

  private:
    // statements
    LiteralValue *evaluate_variable_declaration(VariableDeclaration *v,
                                                Scope *scope);

    LiteralValue *evaluate_function_call(CallExpr *s, Scope *scope);
    LiteralValue *evaluate_function_definition(FunctionDefExpr *s,
                                               Scope *scope);
    LiteralValue *evaluate_class_definition(ClassDefinitionExpr *s,
                                            Scope *scope);
    LiteralValue *evaluate_object_instantiation(ObjectInstantiationExpr *s,
                                                Scope *scope);
    LiteralValue *evaluate_object_attr_reassign(ObjectAttrReassignExpr *s,
                                                Scope *scope);

    LiteralValue *evaluate_if_statement(IfExpr *s, Scope *scope);
    LiteralValue *evaluate_while_statement(WhileExpr *s, Scope *scope);
    LiteralValue *evaluate_return_statement(ReturnExpr *s, Scope *scope);
    LiteralValue *evaluate_list(ListExpr *s, Scope *scope);

    // expressions
    LiteralValue *evaluate_expr(Expr *expr, Scope *scope);
    LiteralValue *evaluate_binary_expr(BinaryExpr *v, Scope *scope);
    LiteralValue *evaluate_unary_expr(UnaryExpr *v, Scope *scope);

    LiteralValue *evaluate_dot_expr(DotExpr *s, Scope *scope);
    LiteralValue *get_variable(const std::string &s, Scope *scope);

    // INFO: predefined STDIO functions
    LiteralValue *print(CallExpr *s, Scope *scope);
    LiteralValue *input(CallExpr *s, Scope *scope);

    Scope global_scope;
    Memory memory;
};

#endif // RUNTIME_INTERPETER_HPP
