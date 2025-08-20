#include "runtime.hpp"

#include <stdexcept>

#include "ast.hpp"
#include "runtime/value.hpp"

Runtime::Runtime() {}

LiteralValue *Runtime::get_variable_value(const std::string &name) {
    if (var_exists(name)) return variables[name];
    throw std::runtime_error("Error: Variable name: '" + name +
                             "' does not exist.\n");
}

void Runtime::var_define(const std::string &name, LiteralValue *value) {
    variables[name] = value;
}

bool Runtime::var_exists(const std::string &name) {
    return variables.contains(name);
}

FunctionDefExpr *Runtime::get_func_value(const std::string &name) {
    if (func_exists(name)) return functions[name];
    throw std::runtime_error("Error: Function name: '" + name +
                             "' does not exist.\n");
}

void Runtime::func_define(const std::string &name, FunctionDefExpr *expr) {
    functions[name] = expr;
}

bool Runtime::func_exists(const std::string &name) {
    return functions.contains(name);
}

ClassDefinitionExpr *Runtime::get_class_value(const std::string &name) {
    if (class_exists(name)) return class_definitions[name];
    throw std::runtime_error("Error: Class name: '" + name +
                             "' does not exist.\n");
}

void Runtime::class_define(const std::string &name, ClassDefinitionExpr *expr) {
    class_definitions[name] = expr;
}

bool Runtime::class_exists(const std::string &name) {
    return class_definitions.contains(name);
}
