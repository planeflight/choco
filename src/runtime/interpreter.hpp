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
    LiteralValue *evaluate(Statement *statement) {
        switch (statement->type) {
            case ASTNodeType::VARIABLE_DECLARATION: {
                VariableDeclaration *v =
                    dynamic_cast<VariableDeclaration *>(statement);
                return evaluate_variable_declaration(v);
            }
            case ASTNodeType::FUNCTION_CALL: {
                CallExpr *v = dynamic_cast<CallExpr *>(statement);
                return evaluate_function_call(v);
            }
            case ASTNodeType::BINARY: {
                BinaryExpr *v = dynamic_cast<BinaryExpr *>(statement);
                return evaluate_binary_expr(v);
            }
        }
        UNIMPLEMENTED();
    }

    LiteralValue *evaluate_variable_declaration(VariableDeclaration *v);

    LiteralValue *evaluate_function_call(CallExpr *s) {
        // TODO: check if function name is user defined

        // check if function name is defined in STD spec
        if (s->name == "print") {
            return print(s);
        }
        UNIMPLEMENTED();
    }

    LiteralValue *evaluate_binary_expr(BinaryExpr *v);
    LiteralValue *evaluate_unary_expr(UnaryExpr *v);
    LiteralValue *evaluate_expr(Expr *expr);

    // INFO: predefined STDIO functions
    LiteralValue *print(CallExpr *s);

  private:
    Runtime global_runtime;
    Memory memory;
};

#endif // RUNTIME_INTERPETER_HPP
