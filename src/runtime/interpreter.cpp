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
    return memory.get<NoneValue>();
}

LiteralValue *Interpreter::evaluate_variable_declaration(
    VariableDeclaration *v) {
    // should have caught all syntax/grammar errors
    const auto &name = v->name;
    if (!global_runtime.exists(name)) {
        global_runtime.define(name, evaluate_expr(v->value.get()));
        return memory.get<NoneValue>();
    }
    throw std::runtime_error("Error: Variable name '" + name +
                             "' already declared.\n");
}

LiteralValue *Interpreter::evaluate_expr(Expr *expr) {
    if (expr->type == ASTNodeType::LITERAL) {
        return static_cast<LiteralExpr *>(expr)->value.get();
    }
    if (expr->type == ASTNodeType::SYMBOL) {
        return global_runtime.get_variable_value(
            (static_cast<SymbolExpr *>(expr))->symbol);
    }
    if (expr->type == ASTNodeType::BINARY) {
        return evaluate_binary_expr(static_cast<BinaryExpr *>(expr));
    }
    if (expr->type == ASTNodeType::UNARY) {
        return evaluate_unary_expr(static_cast<UnaryExpr *>(expr));
    }
    return memory.get<NoneValue>();
}

LiteralValue *Interpreter::evaluate_binary_expr(BinaryExpr *v) {
    auto left = evaluate_expr(v->left.get());
    auto right = evaluate_expr(v->right.get());
    if (left->type == ValueType::NUMBER && right->type == ValueType::NUMBER) {
        auto left_num_value = static_cast<NumValue *>(left);
        auto right_num_value = static_cast<NumValue *>(right);
        NumValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<NumValue>(left_num_value->value +
                                       right_num_value->value);
        } else if (v->op == TokenType::MINUS) {
            res = memory.get<NumValue>(left_num_value->value -
                                       right_num_value->value);
        } else if (v->op == TokenType::MUL) {
            res = memory.get<NumValue>(left_num_value->value *
                                       right_num_value->value);
        } else if (v->op == TokenType::DIV) {
            if (right_num_value->value != 0.0)
                res = memory.get<NumValue>(left_num_value->value /
                                           right_num_value->value);
            else
                res = memory.get<NumValue>(0.0);
        } else {
            throw new std::runtime_error("Error: Invalid numerical operation.");
        }
        return res;
    }

    if (left->type == ValueType::STRING && right->type == ValueType::STRING) {
        auto lval = static_cast<StringValue *>(left);
        auto rval = static_cast<StringValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>(lval->value + rval->value);
            return res;
        }
        throw new std::runtime_error("Error: Invalid string operation.");
    }
    // TODO: different string concatenation with string + num or string + bool,
    // etc
    return nullptr;
}

LiteralValue *Interpreter::evaluate_unary_expr(UnaryExpr *v) {
    auto left = evaluate_expr(v->unary.get());
    // -<expr>
    if (left->type == ValueType::NUMBER && v->op == TokenType::MINUS) {
        // INFO: don't alter NumValue-> value directly because it might change
        // some variable's value, need a copy
        NumValue *res =
            memory.get<NumValue>(-static_cast<NumValue *>(left)->value);
        return res;
    }
    // !<bool-expr>
    else if (left->type == ValueType::BOOL && v->op == TokenType::NOT) {
        BoolValue *res =
            memory.get<BoolValue>(!static_cast<BoolValue *>(left)->value);
        return res;
    }
    return nullptr;
}
