#include "interpreter.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "ast.hpp"
#include "runtime/runtime.hpp"
#include "runtime/scope.hpp"
#include "token.hpp"
#include "value.hpp"

Interpreter::Interpreter() {}

void Interpreter::eval(const std::vector<uptr<Statement>> &ast) {
    for (const auto &s : ast) {
        evaluate(s.get(), &global_scope);
    }
}

LiteralValue *Interpreter::evaluate(Statement *statement, Scope *scope) {
    switch (statement->type) {
        case ASTNodeType::VARIABLE_REASSIGN:
        case ASTNodeType::VARIABLE_DECLARATION: {
            VariableDeclaration *v =
                dynamic_cast<VariableDeclaration *>(statement);
            return evaluate_variable_declaration(v, scope);
        }
        case ASTNodeType::FUNCTION_CALL: {
            CallExpr *v = dynamic_cast<CallExpr *>(statement);
            return evaluate_function_call(v, scope);
        }
        case ASTNodeType::IF_STATEMENT: {
            IfExpr *v = dynamic_cast<IfExpr *>(statement);
            return evaluate_if_statement(v, scope);
        }
        case ASTNodeType::BINARY: {
            BinaryExpr *v = dynamic_cast<BinaryExpr *>(statement);
            return evaluate_binary_expr(v, scope);
        }
        case ASTNodeType::WHILE_STATEMENT: {
            WhileExpr *v = dynamic_cast<WhileExpr *>(statement);
            return evaluate_while_statement(v, scope);
        }
        case ASTNodeType::FUNCTION_DEFINITION: {
            FunctionDefExpr *v = dynamic_cast<FunctionDefExpr *>(statement);
            return evaluate_function_definition(v, scope);
        }
        case ASTNodeType::RETURN_STATEMENT: {
            ReturnExpr *v = dynamic_cast<ReturnExpr *>(statement);
            return evaluate_return_statement(v, scope);
        }
        case ASTNodeType::CLASS_DEFINITION: {
            ClassDefinitionExpr *v =
                dynamic_cast<ClassDefinitionExpr *>(statement);
            return evaluate_class_definition(v, scope);
        }
        case ASTNodeType::LIST: {
            ListExpr *v = dynamic_cast<ListExpr *>(statement);
            return evaluate_list(v, scope);
        }
    }
    return nullptr;
    UNIMPLEMENTED();
}

LiteralValue *Interpreter::evaluate_variable_declaration(VariableDeclaration *v,
                                                         Scope *scope) {
    // should have caught all syntax/grammar errors
    // TODO: define the variable at the appropriate scope
    const auto &name = v->name;
    if (v->type == ASTNodeType::VARIABLE_DECLARATION) {
        if (!scope->runtime.var_exists(name)) {
            scope->runtime.var_define(name,
                                      evaluate_expr(v->value.get(), scope));
            return nullptr;
        }
        throw std::runtime_error("Error: Variable name '" + name +
                                 "' already declared.\n");
    }
    // variable reassignment
    else {
        if (!global_scope.runtime.var_exists(name)) {
            throw std::runtime_error("Error: Variable name '" + name +
                                     "' does not exist!");
        }
        global_scope.runtime.var_define(name,
                                        evaluate_expr(v->value.get(), scope));
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_function_call(CallExpr *s, Scope *scope) {
    // check if function name is defined in STD spec
    if (s->callee->symbol == "print") {
        return print(s, scope);
    }
    if (s->callee->symbol == "input") {
        return input(s, scope);
    }
    // check in user-defined functions
    if (global_scope.runtime.func_exists(s->callee->symbol)) {
        FunctionDefExpr *function =
            global_scope.runtime.get_func_value(s->callee->symbol);

        if (function->params.size() != s->params.size()) {
            throw std::runtime_error(
                "Error: Function definition '" + s->callee->symbol + "' has " +
                std::to_string(function->params.size()) +
                " parameters, but got " + std::to_string(s->params.size()) +
                " arguments.");
        }

        // give the values to the parameters in a new scope
        Scope local_scope;
        local_scope.parent = &global_scope;
        for (int i = 0; i < function->params.size(); ++i) {
            LiteralValue *val = evaluate_expr(s->params[i].get(), scope);
            local_scope.runtime.var_define(function->params[i], val);
        }
        LiteralValue *result;
        // evaluate the function with the given parameters
        for (const auto &s : function->statements) {
            result = evaluate(s.get(), &local_scope);
            if (result) return result;
        }
        return nullptr;
    }
    return nullptr;

    UNIMPLEMENTED();
}

LiteralValue *Interpreter::evaluate_function_definition(FunctionDefExpr *s,
                                                        Scope *scope) {
    if (!global_scope.runtime.func_exists(s->name)) {
        global_scope.runtime.func_define(s->name, s);
        return nullptr;
    }
    throw std::runtime_error("Error: Function name '" + s->name +
                             "' already declared.\n");
}

LiteralValue *Interpreter::evaluate_class_definition(ClassDefinitionExpr *s,
                                                     Scope *scope) {
    if (!global_scope.runtime.class_exists(s->name)) {
        global_scope.runtime.class_define(s->name, s);
        return nullptr;
    }
    throw std::runtime_error("Error: Class name '" + s->name +
                             "' already declared.\n");
    return nullptr;
}

LiteralValue *Interpreter::evaluate_if_statement(IfExpr *s, Scope *scope) {
    bool run_else = true;
    auto eval =
        static_cast<BoolValue *>(evaluate_expr(s->condition.get(), scope));
    if (eval->value) {
        run_else = false;
        Scope new_scope;
        new_scope.parent = scope;

        for (const auto &s : s->statements) {
            evaluate(s.get(), &new_scope);
        }
    } else {
        for (const auto &elif : s->elif_statements) {
            Scope new_scope;
            new_scope.parent = scope;

            eval = static_cast<BoolValue *>(
                evaluate_expr(elif->condition.get(), &new_scope));
            if (eval->value) {
                run_else = false;
                for (const auto &s : elif->statements) {
                    evaluate(s.get(), &new_scope);
                }
                break;
            }
        }
        if (run_else) {
            Scope new_scope;
            new_scope.parent = scope;
            for (const auto &s : s->else_statements) {
                evaluate(s.get(), &new_scope);
            }
        }
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_while_statement(WhileExpr *s,
                                                    Scope *scope) {
    auto eval =
        static_cast<BoolValue *>(evaluate_expr(s->condition.get(), scope));
    Scope new_scope;
    new_scope.parent = scope;
    while (eval->value) {
        // TODO: break/continue
        for (const auto &s : s->statements) {
            evaluate(s.get(), &new_scope);
        }
        // recalculate eval
        eval =
            static_cast<BoolValue *>(evaluate_expr(s->condition.get(), scope));
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_return_statement(ReturnExpr *s,
                                                     Scope *scope) {
    return evaluate_expr(s->content.get(), scope);
}

LiteralValue *Interpreter::evaluate_list(ListExpr *s, Scope *scope) {
    auto list = memory.get<ListValue>();
    for (auto &element : s->elements) {
        auto eval = evaluate_expr(element.get(), scope);
        list->value.push_back(eval);
    }
    return list;
}

LiteralValue *Interpreter::evaluate_expr(Expr *expr, Scope *scope) {
    if (expr->type == ASTNodeType::LITERAL) {
        return static_cast<LiteralExpr *>(expr)->value.get();
    }
    if (expr->type == ASTNodeType::LIST) {
        return evaluate_list(static_cast<ListExpr *>(expr), scope);
    }
    if (expr->type == ASTNodeType::SYMBOL) {
        // check scopes
        const std::string &name = (static_cast<SymbolExpr *>(expr))->symbol;
        while (scope != nullptr) {
            if (scope->runtime.var_exists(name)) {
                return scope->runtime.get_variable_value(name);
            }
            // move up the tree
            scope = scope->parent;
        }
        throw std::runtime_error("Error: Variable '" + name +
                                 "' is not in scope and does not exist!");
    }
    if (expr->type == ASTNodeType::BINARY) {
        return evaluate_binary_expr(static_cast<BinaryExpr *>(expr), scope);
    }
    if (expr->type == ASTNodeType::UNARY) {
        return evaluate_unary_expr(static_cast<UnaryExpr *>(expr), scope);
    }
    if (expr->type == ASTNodeType::FUNCTION_CALL) {
        return evaluate_function_call(static_cast<CallExpr *>(expr), scope);
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_binary_expr(BinaryExpr *v, Scope *scope) {
    auto left = evaluate_expr(v->left.get(), scope);
    auto right = evaluate_expr(v->right.get(), scope);
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

LiteralValue *Interpreter::evaluate_unary_expr(UnaryExpr *v, Scope *scope) {
    auto left = evaluate_expr(v->unary.get(), scope);
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

LiteralValue *Interpreter::print(CallExpr *s, Scope *scope) {
    if (s->params.size() == 0) {
        std::cout << "\n";
    } else {
        auto *child = s->params.front().get();
        LiteralValue *v = evaluate_expr(child, scope);
        if (v) {
            std::cout << literal_to_string(*v) << std::endl;
        } else
            throw std::runtime_error("Error: Invalid value!");
    }
    return nullptr;
}

LiteralValue *Interpreter::input(CallExpr *s, Scope *scope) {
    // print the prompt
    print(s, scope);
    // get the different values
    std::string in;
    std::cin >> in;
    return memory.get<StringValue>(in);
}
