#include "interpreter.hpp"

#include <stdexcept>

#include "ast.hpp"
#include "token.hpp"
#include "value.hpp"

Interpreter::Interpreter() {}

LiteralValue *Interpreter::print(CallExpr *s) {
    if (s->params.size() == 0) {
        std::cout << "\n";
    } else {
        auto *child = s->params.front().get();
        LiteralValue *v = evaluate_expr(child);
        std::cout << literal_to_string(*v) << std::endl;
    }
    return new NoneValue{};
}

LiteralValue *Interpreter::evaluate_variable_declaration(
    VariableDeclaration *v) {
    // should have caught all syntax/grammar errors
    const auto &name = v->name;
    if (!global_runtime.exists(name)) {
        global_runtime.define(name, evaluate_expr(v->value.get()));
        return new NoneValue{};
    }
    throw std::runtime_error("Error: Variable name '" + name +
                             "' already declared.\n");
}

LiteralValue *Interpreter::evaluate_expr(Expr *expr) {
    if (auto e = dynamic_cast<LiteralExpr *>(expr)) {
        return e->value.get();
    }
    if (auto e = dynamic_cast<SymbolExpr *>(expr)) {
        return global_runtime.get_variable_value(e->symbol);
    }
    return new NoneValue{};
}
