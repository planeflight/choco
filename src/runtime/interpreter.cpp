#include "interpreter.hpp"

#include <iostream>
#include <stdexcept>

#include "ast.hpp"
#include "token.hpp"
#include "value.hpp"

Interpreter::Interpreter() {}

LiteralValue *Interpreter::evaluate(Statement *statement) {
    switch (statement->type) {
        case ASTNodeType::VARIABLE_REASSIGN:
        case ASTNodeType::VARIABLE_DECLARATION: {
            VariableDeclaration *v =
                dynamic_cast<VariableDeclaration *>(statement);
            return evaluate_variable_declaration(v);
        }
        case ASTNodeType::FUNCTION_CALL: {
            CallExpr *v = dynamic_cast<CallExpr *>(statement);
            return evaluate_function_call(v);
        }
        case ASTNodeType::IF_STATEMENT: {
            IfExpr *v = dynamic_cast<IfExpr *>(statement);
            return evaluate_if_statement(v);
        }
        case ASTNodeType::BINARY: {
            BinaryExpr *v = dynamic_cast<BinaryExpr *>(statement);
            return evaluate_binary_expr(v);
        }
        case ASTNodeType::WHILE_STATEMENT: {
            WhileExpr *v = dynamic_cast<WhileExpr *>(statement);
            return evaluate_while_statement(v);
        }
    }
    UNIMPLEMENTED();
}

LiteralValue *Interpreter::evaluate_variable_declaration(
    VariableDeclaration *v) {
    // should have caught all syntax/grammar errors
    const auto &name = v->name;
    if (v->type == ASTNodeType::VARIABLE_DECLARATION) {
        if (!global_runtime.exists(name)) {
            global_runtime.define(name, evaluate_expr(v->value.get()));
            return memory.get<NoneValue>();
        }
        throw std::runtime_error("Error: Variable name '" + name +
                                 "' already declared.\n");
    }
    if (!global_runtime.exists(name)) {
        throw std::runtime_error("Error: Variable name '" + name +
                                 "' does not exist!");
    }
    global_runtime.define(name, evaluate_expr(v->value.get()));
    return memory.get<NoneValue>();
}

LiteralValue *Interpreter::evaluate_function_call(CallExpr *s) {
    // TODO: check if function name is user defined

    // check if function name is defined in STD spec
    if (s->callee->symbol == "print") {
        return print(s);
    }
    if (s->callee->symbol == "input") {
        return input(s);
    }
    UNIMPLEMENTED();
}

LiteralValue *Interpreter::evaluate_if_statement(IfExpr *s) {
    bool run_else = true;
    auto eval = static_cast<BoolValue *>(evaluate_expr(s->condition.get()));
    if (eval->value) {
        run_else = false;
        for (const auto &s : s->statements) {
            evaluate(s.get());
        }
    } else {
        for (const auto &elif : s->elif_statements) {
            eval =
                static_cast<BoolValue *>(evaluate_expr(elif->condition.get()));
            if (eval->value) {
                run_else = false;
                for (const auto &s : elif->statements) {
                    evaluate(s.get());
                }
                break;
            }
        }
        if (run_else) {
            for (const auto &s : s->else_statements) {
                evaluate(s.get());
            }
        }
    }
    return memory.get<BoolValue>(eval->value);
}

LiteralValue *Interpreter::evaluate_while_statement(WhileExpr *s) {
    auto eval = static_cast<BoolValue *>(evaluate_expr(s->condition.get()));
    while (eval->value) {
        // TODO: break/continue
        for (const auto &s : s->statements) {
            evaluate(s.get());
        }
        // recalculate eval
        eval = static_cast<BoolValue *>(evaluate_expr(s->condition.get()));
    }
    return memory.get<NoneValue>();
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
    if (expr->type == ASTNodeType::FUNCTION_CALL) {
        return evaluate_function_call(static_cast<CallExpr *>(expr));
    }
    return memory.get<NoneValue>();
}

LiteralValue *Interpreter::evaluate_binary_expr(BinaryExpr *v) {
    auto left = evaluate_expr(v->left.get());
    auto right = evaluate_expr(v->right.get());
    if (left->type == ValueType::NUMBER && right->type == ValueType::NUMBER) {
        auto lval = static_cast<NumValue *>(left);
        auto rval = static_cast<NumValue *>(right);
        NumValue *res_num = memory.get<NumValue>(0.0);
        BoolValue *res_bool = memory.get<BoolValue>(0.0);
        // arithmetic operators
        if (v->op == TokenType::PLUS) {
            res_num->value = lval->value + rval->value;
            return res_num;
        } else if (v->op == TokenType::MINUS) {
            res_num->value = lval->value - rval->value;
            return res_num;
        } else if (v->op == TokenType::MUL) {
            res_num->value = lval->value * rval->value;
            return res_num;
        } else if (v->op == TokenType::DIV) {
            if (rval->value != 0.0)
                res_num->value = lval->value / rval->value;
            else
                res_num->value = 0.0;
            return res_num;
        }
        // boolean operators
        else if (v->op == TokenType::LT) {
            res_bool->value = lval->value < rval->value;
        } else if (v->op == TokenType::GT) {
            res_bool->value = lval->value > rval->value;
        } else if (v->op == TokenType::LOT) {
            res_bool->value = lval->value <= rval->value;
        } else if (v->op == TokenType::GOT) {
            res_bool->value = lval->value >= rval->value;
        } else if (v->op == TokenType::EQUALS) {
            res_bool->value = lval->value == rval->value;
        } else {
            throw std::runtime_error("Error: Invalid numerical operation.");
        }
        return res_bool;
    }

    if (left->type == ValueType::STRING && right->type == ValueType::STRING) {
        auto lval = static_cast<StringValue *>(left);
        auto rval = static_cast<StringValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>(lval->value + rval->value);
            return res;
        }
        throw std::runtime_error("Error: Invalid string operation.");
    }
    // TODO: different string concatenation with string + num or string + bool,
    // etc
    // string concatenation
    if (left->type == ValueType::STRING && right->type == ValueType::NUMBER) {
        auto lval = static_cast<StringValue *>(left);
        auto rval = static_cast<NumValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>(lval->value +
                                          std::to_string(rval->value));
            return res;
        }
        throw std::runtime_error("Error: Invalid string operation.");
    }
    if (left->type == ValueType::NUMBER && right->type == ValueType::STRING) {
        auto lval = static_cast<NumValue *>(left);
        auto rval = static_cast<StringValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>(std::to_string(lval->value) +
                                          rval->value);
            return res;
        }
        throw std::runtime_error("Error: Invalid string operation.");
    }

    if (left->type == ValueType::STRING && right->type == ValueType::BOOL) {
        auto lval = static_cast<StringValue *>(left);
        auto rval = static_cast<BoolValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>(lval->value +
                                          (rval->value ? "true" : "false"));
            return res;
        }
        throw std::runtime_error("Error: Invalid string operation.");
    }
    if (left->type == ValueType::BOOL && right->type == ValueType::STRING) {
        auto lval = static_cast<BoolValue *>(left);
        auto rval = static_cast<StringValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            res = memory.get<StringValue>((lval->value ? "true" : "false") +
                                          rval->value);
            return res;
        }
        throw std::runtime_error("Error: Invalid string operation.");
    }

    // comparison/boolean operators
    if (left->type == ValueType::BOOL && right->type == ValueType::BOOL) {
        auto lval = static_cast<BoolValue *>(left);
        auto rval = static_cast<BoolValue *>(right);
        BoolValue *res = memory.get<BoolValue>(false);
        if (v->op == TokenType::AND) {
            res->value = lval && rval;
        } else if (v->op == TokenType::OR) {
            res->value = lval || rval;

        } else if (v->op == TokenType::EQUALS) {
            res->value = lval == rval;
        } else if (v->op == TokenType::NOT_EQUAL) {
            res->value = lval != rval;
        }
        return res;
    }

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

LiteralValue *Interpreter::input(CallExpr *s) {
    // print the prompt
    print(s);
    // get the different values
    std::string in;
    std::cin >> in;
    return memory.get<StringValue>(in);
}
