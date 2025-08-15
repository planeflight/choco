#include "runtime.hpp"

#include <stdexcept>

#include "runtime/value.hpp"

Runtime::Runtime() {}

LiteralValue *Runtime::get_variable_value(const std::string &name) {
    if (exists(name)) return variables[name];
    throw std::runtime_error("Error: Variable name: '" + name +
                             "' does not exist.\n");
}

void Runtime::define(const std::string &name, LiteralValue *value) {
    variables[name] = value;
}

bool Runtime::exists(const std::string &name) {
    return variables.contains(name);
}
