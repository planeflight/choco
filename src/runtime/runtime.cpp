#include "runtime.hpp"

#include <stdexcept>

#include "ast.hpp"
#include "runtime/value.hpp"
#include "util/error.hpp"

Runtime::Runtime() {}

LiteralValue *Runtime::get_variable_value(const std::string &name) {
    if (var_exists(name)) return variables[name];
    throw Error(Error::NAME_ERROR,
                fmt::format("Variable name: '{}' does not exist.", name));
}

void Runtime::var_define(const std::string &name, LiteralValue *value) {
    variables[name] = value;
}

bool Runtime::var_exists(const std::string &name) {
    return variables.contains(name);
}

FunctionDefExpr *Runtime::get_func_value(const std::string &name) {
    if (func_exists(name)) return functions[name];
    throw Error(Error::NAME_ERROR,
                fmt::format("Function name: '{}' does not exist.", name));
}

void Runtime::func_define(const std::string &name, FunctionDefExpr *expr) {
    functions[name] = expr;
}

bool Runtime::func_exists(const std::string &name) {
    return functions.contains(name);
}

ClassDefinitionExpr *Runtime::get_class_value(const std::string &name) {
    if (class_exists(name)) return class_definitions[name];
    throw Error(Error::NAME_ERROR,
                fmt::format("Class name: '{}' does not exist.", name));
}

void Runtime::class_define(const std::string &name, ClassDefinitionExpr *expr) {
    class_definitions[name] = expr;
}

bool Runtime::class_exists(const std::string &name) {
    return class_definitions.contains(name);
}
