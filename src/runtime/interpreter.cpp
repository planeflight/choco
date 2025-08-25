#include "interpreter.hpp"

#include <fmt/core.h>
#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include "ast.hpp"
#include "runtime/graphics.hpp"
#include "runtime/runtime.hpp"
#include "runtime/scope.hpp"
#include "token.hpp"
#include "util/error.hpp"
#include "util/util.hpp"
#include "value.hpp"

LiteralValue *copy(Memory &memory, LiteralValue *s) {
    switch (s->type) {
        case ValueType::NUMBER: {
            return memory.get<NumValue>(as_double(s));
        }
        case ValueType::BOOL: {
            return memory.get<BoolValue>(as_bool(s));
        }
        case ValueType::STRING: {
            return memory.get<StringValue>(as_string(s));
        }
        // list and objects are references
        case ValueType::LIST: {
            return s;
        }
        case ValueType::OBJECT: {
            return s;
        }
    }
}

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
        case ASTNodeType::OBJECT_ATTR_REASSIGN: {
            ObjectAttrReassignExpr *v =
                dynamic_cast<ObjectAttrReassignExpr *>(statement);
            return evaluate_object_attr_reassign(v, scope);
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
    const auto &name = v->name;
    if (v->type == ASTNodeType::VARIABLE_DECLARATION) {
        if (!scope->runtime.var_exists(name)) {
            scope->runtime.var_define(name,
                                      evaluate_expr(v->value.get(), scope));
            return nullptr;
        }
        throw Error(
            Error::NAME_ERROR,
            fmt::format("Error: Variable name '{}' already declared.", name));
    }
    // variable reassignment
    else {
        // track the original scope for evaluation of expression
        Scope *curr_scope = scope;
        while (!scope->runtime.var_exists(name)) {
            if (scope->parent != nullptr) {
                scope = scope->parent;
            } else {
                throw Error(
                    Error::NAME_ERROR,
                    fmt::format("Error: Variable name '{}' does not exist!\n",
                                name));
            }
        }
        scope->runtime.var_define(name,
                                  evaluate_expr(v->value.get(), curr_scope));
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
    std::vector<LiteralValue *> args;
    std::for_each(s->params.begin(),
                  s->params.end(),
                  [&args, this, &scope](uptr<Expr> &expr) {
                      args.push_back(evaluate_expr(expr.get(), scope));
                  });

    args.reserve(s->params.size());
    // math funcs
    if (s->callee->symbol == "abs") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::abs(as_double(args[0])));
    }
    if (s->callee->symbol == "sign") {
        expect_args(args, 1);
        double v = as_double(args[0]);
        return memory.get<NumValue>(v / std::abs(v));
    }
    if (s->callee->symbol == "pow") {
        expect_args(args, 2);
        return memory.get<NumValue>(
            std::pow(as_double(args[0]), as_double(args[1])));
    }
    if (s->callee->symbol == "sin") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::sin(as_double(args[0])));
    }
    if (s->callee->symbol == "cos") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::cos(as_double(args[0])));
    }
    if (s->callee->symbol == "tan") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::tan(as_double(args[0])));
    }
    if (s->callee->symbol == "sqrt") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::sqrt(as_double(args[0])));
    }
    if (s->callee->symbol == "round") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::round(as_double(args[0])));
    }
    if (s->callee->symbol == "ceil") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::ceil(as_double(args[0])));
    }
    if (s->callee->symbol == "floor") {
        expect_args(args, 1);
        return memory.get<NumValue>(std::floor(as_double(args[0])));
    }
    // array/string methods
    if (s->callee->symbol == "len") {
        expect_args(args, 1);
        auto v = args[0];
        if (v->type == ValueType::LIST)
            return memory.get<NumValue>(
                static_cast<ListValue *>(v)->value.size());
        if (v->type == ValueType::STRING)
            return memory.get<NumValue>(
                static_cast<StringValue *>(v)->value.size());
        throw Error(Error::INVALID_ARGUMENT_ERROR,
                    "Expected list or string type.");
    }
    if (s->callee->symbol == "get") {
        expect_args(args, 2);
        auto container = args[0];
        int idx = (int)as_double(args[1]);
        if (container->type == ValueType::LIST)
            return static_cast<ListValue *>(container)->value[idx];
        if (container->type == ValueType::STRING) {
            std::string str =
                std::string{static_cast<StringValue *>(container)->value[idx]};
            return memory.get<StringValue>(str);
        }
        throw Error(Error::INVALID_ARGUMENT_ERROR,
                    "Expected list or string type.");
    }
    if (s->callee->symbol == "append") {
        expect_args(args, 2);
        auto container = args[0];
        auto v = args[1];
        if (container->type == ValueType::LIST) {
            static_cast<ListValue *>(container)->value.push_back(v);
            return nullptr;
        }
        if (container->type == ValueType::STRING) {
            const std::string &str_to_add = as_string(v);
            static_cast<StringValue *>(container)->value.append(str_to_add);
            return nullptr;
        }
        throw Error(Error::INVALID_ARGUMENT_ERROR,
                    "Expected list or string type.");
    }
    if (s->callee->symbol == "pop") {
        expect_args(args, 1);
        auto container = args[0];
        if (container->type == ValueType::LIST) {
            static_cast<ListValue *>(container)->value.pop_back();
            return nullptr;
        }
        if (container->type == ValueType::STRING) {
            static_cast<StringValue *>(container)->value.pop_back();
            return nullptr;
        }
        throw Error(Error::INVALID_ARGUMENT_ERROR,
                    "Expected list or string type.");
    }
    if (s->callee->symbol == "insert") {
        expect_args(args, 3);
        auto container = args[0];
        int idx = (int)as_double(args[1]);
        auto elem = args[2];
        if (container->type == ValueType::LIST) {
            auto &vec = static_cast<ListValue *>(container)->value;
            vec.insert(vec.begin() + idx, elem);
            return nullptr;
        }
        if (container->type == ValueType::STRING) {
            const std::string &str_to_add = as_string(elem);
            auto &str = static_cast<StringValue *>(container)->value;
            str.insert(idx, str_to_add);
            return nullptr;
        }
        throw Error(Error::INVALID_ARGUMENT_ERROR,
                    "Expected list or string type.");
    }

    // GRAPHICS RAYLIB FUNCTIONS
    if (s->callee->symbol == "init_window") return init_window(memory, args);
    if (s->callee->symbol == "close_window") return close_window(memory, args);
    if (s->callee->symbol == "window_should_close")
        return window_should_close(memory, args);
    if (s->callee->symbol == "begin_drawing")
        return begin_drawing(memory, args);
    if (s->callee->symbol == "end_drawing") return end_drawing(memory, args);
    if (s->callee->symbol == "clear_background")
        return clear_background(memory, args);
    if (s->callee->symbol == "draw_rectangle")
        return draw_rectangle(memory, args);
    if (s->callee->symbol == "draw_text") return draw_text(memory, args);
    if (s->callee->symbol == "draw_circle") return draw_circle(memory, args);
    if (s->callee->symbol == "is_key_down") return is_key_down(memory, args);

    // check in user-defined functions
    if (global_scope.runtime.func_exists(s->callee->symbol)) {
        FunctionDefExpr *function =
            global_scope.runtime.get_func_value(s->callee->symbol);

        if (function->params.size() != s->params.size()) {
            throw Error(Error::INVALID_ARGUMENT_ERROR,
                        fmt::format("Error: Function definition '{}' has {} "
                                    "parameters but got {} arguments.",
                                    s->callee->symbol,
                                    function->params.size(),
                                    s->params.size()));
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
            if (result) {
                return result;
            }
        }
        return nullptr;
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_function_definition(FunctionDefExpr *s,
                                                        Scope *scope) {
    if (!global_scope.runtime.func_exists(s->name)) {
        global_scope.runtime.func_define(s->name, s);
        return nullptr;
    }
    throw Error(
        Error::NAME_ERROR,
        fmt::format("Error: Function name '{}' already declared.", s->name));
}

LiteralValue *Interpreter::evaluate_class_definition(ClassDefinitionExpr *s,
                                                     Scope *scope) {
    if (!global_scope.runtime.class_exists(s->name)) {
        global_scope.runtime.class_define(s->name, s);
        return nullptr;
    }
    throw Error(
        Error::NAME_ERROR,
        fmt::format("Error: Class name '{}' already declared.", s->name));
    return nullptr;
}

LiteralValue *Interpreter::evaluate_object_instantiation(
    ObjectInstantiationExpr *s,
    Scope *scope) {
    // check if class name already exists, if not error
    if (!global_scope.runtime.class_exists(s->class_name)) {
        throw Error(Error::NAME_ERROR,
                    fmt::format("Error: Class name '{}' does not exist.",
                                s->class_name));
    }

    ClassDefinitionExpr *class_definition =
        global_scope.runtime.get_class_value(s->class_name);
    auto new_obj_value = memory.get<ObjectValue>();
    // set default values, then call constructor
    for (auto &attr : class_definition->attributes) {
        LiteralValue *s = evaluate_expr(attr->value.get(), scope);
        new_obj_value->values[attr->name] = copy(memory, s);
    }
    return new_obj_value;
}

LiteralValue *Interpreter::evaluate_object_attr_reassign(
    ObjectAttrReassignExpr *s,
    Scope *scope) {
    LiteralValue *eval_dot =
        evaluate_dot_expr(static_cast<DotExpr *>(s->head.get()), scope);
    *eval_dot = *evaluate_expr(s->right.get(), scope);
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
            auto v = evaluate(s.get(), &new_scope);
            if (v) return v;
        }
    } else {
        for (const auto &elif : s->elif_statements) {
            eval = static_cast<BoolValue *>(
                evaluate_expr(elif->condition.get(), scope));
            if (eval->value) {
                // new scope for each elif
                Scope new_scope;
                new_scope.parent = scope;

                run_else = false;
                for (const auto &s : elif->statements) {
                    auto v = evaluate(s.get(), &new_scope);
                    if (v) return v;
                }
                break;
            }
        }
        if (run_else) {
            Scope new_scope;
            new_scope.parent = scope;
            for (const auto &s : s->else_statements) {
                auto v = evaluate(s.get(), &new_scope);
                if (v) return v;
            }
        }
    }
    return nullptr;
}

LiteralValue *Interpreter::evaluate_while_statement(WhileExpr *s,
                                                    Scope *scope) {
    auto eval =
        static_cast<BoolValue *>(evaluate_expr(s->condition.get(), scope));
    while (eval->value) {
        Scope new_scope;
        new_scope.parent = scope;
        // TODO: break/continue
        for (const auto &s : s->statements) {
            auto v = evaluate(s.get(), &new_scope);
            if (v) return v;
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
        return get_variable(name, scope);
    }
    if (expr->type == ASTNodeType::DOT_SYMBOL) {
        // check scopes
        auto dot = static_cast<DotExpr *>(expr);
        return evaluate_dot_expr(dot, scope);
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
    if (expr->type == ASTNodeType::OBJECT_INSTANTIATION) {
        return evaluate_object_instantiation(
            static_cast<ObjectInstantiationExpr *>(expr), scope);
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
        } else if (v->op == TokenType::NOT_EQUAL) {
            res_bool->value = lval->value != rval->value;
        } else {
            throw Error(
                Error::TYPE_ERROR,
                fmt::format("Error: Invalid numerical operation '{}'.", v->op));
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
        if (v->op == TokenType::EQUALS) {
            return memory.get<BoolValue>(lval->value == rval->value);
        }
        throw Error(
            Error::TYPE_ERROR,
            fmt::format("Error: Invalid string operation '{}'.", v->op));
    }
    // string concatenation
    if (left->type == ValueType::STRING && right->type == ValueType::NUMBER) {
        auto lval = static_cast<StringValue *>(left);
        auto rval = static_cast<NumValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            // remove unnecessary digits in string by rounding
            std::string num = std::to_string(rval->value);
            if (std::round(rval->value) == rval->value) {
                num = std::to_string((int)rval->value);
            }
            res = memory.get<StringValue>(lval->value + num);
            return res;
        }
        throw Error(
            Error::TYPE_ERROR,
            fmt::format("Error: Invalid string-number operation '{}'.", v->op));
    }
    if (left->type == ValueType::NUMBER && right->type == ValueType::STRING) {
        auto lval = static_cast<NumValue *>(left);
        auto rval = static_cast<StringValue *>(right);
        StringValue *res = nullptr;
        if (v->op == TokenType::PLUS) {
            // remove unnecessary digits in string by rounding
            std::string num = std::to_string(lval->value);
            if (std::round(lval->value) == lval->value) {
                num = std::to_string((int)lval->value);
            }
            res = memory.get<StringValue>(num + rval->value);
            return res;
        }
        throw Error(
            Error::TYPE_ERROR,
            fmt::format("Error: Invalid number-string operation '{}'.", v->op));
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
        throw Error(
            Error::TYPE_ERROR,
            fmt::format("Error: Invalid string-bool operation '{}'.", v->op));
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
        throw Error(
            Error::TYPE_ERROR,
            fmt::format("Error: Invalid bool-string operation '{}'.", v->op));
    }

    // comparison/boolean operators
    if (left->type == ValueType::BOOL && right->type == ValueType::BOOL) {
        auto lval = static_cast<BoolValue *>(left)->value;
        auto rval = static_cast<BoolValue *>(right)->value;
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
    throw Error(Error::TYPE_ERROR,
                fmt::format("Error: Invalid operation '{}'.", v->op));
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

LiteralValue *Interpreter::evaluate_dot_expr(DotExpr *s, Scope *scope) {
    // get head value
    LiteralValue *head = evaluate_expr(s->head.get(), scope);
    // Object Value to access attributes
    ObjectValue *obj = static_cast<ObjectValue *>(head);
    // Literal Value pointer to track what to return
    LiteralValue *to_return = head;

    for (auto &after : s->after) {
        if (after->type == ASTNodeType::SYMBOL) {
            // WARN: function calls not implemented here
            SymbolExpr *symbol = static_cast<SymbolExpr *>(after.get());
            if (obj->values[symbol->symbol]) {
                to_return = obj->values[symbol->symbol];
                if (to_return->type == ValueType::OBJECT) {
                    obj = static_cast<ObjectValue *>(to_return);
                }
            } else {
                throw Error(Error::NAME_ERROR,
                            "Failed to find symbol after dot expression.");
            }
        } else {
            throw Error(Error::SYNTAX_ERROR,
                        "Invalid function call dot expression.");
        }
    }

    return to_return;
}

LiteralValue *Interpreter::get_variable(const std::string &s, Scope *scope) {
    while (scope != nullptr) {
        if (scope->runtime.var_exists(s)) {
            // returning a reference essentially
            LiteralValue *v = scope->runtime.get_variable_value(s);
            return v;
        } // move up the tree
        scope = scope->parent;
    }
    throw Error(
        Error::NAME_ERROR,
        fmt::format("Variable '{}' is not in scope and does not exist!", s));
}

LiteralValue *Interpreter::print(CallExpr *s, Scope *scope) {
    if (s->params.size() == 0) {
        fmt::println("");
    } else {
        auto *child = s->params.front().get();
        LiteralValue *v = evaluate_expr(child, scope);
        if (v) {
            fmt::println("{}", *v);
        } else
            throw Error(Error::ARGUMENT_ERROR, "Error: Invalid value!");
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
