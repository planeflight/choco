#ifndef RUNTIME_RUNTIME_HPP
#define RUNTIME_RUNTIME_HPP

#include <string>
#include <unordered_map>

#include "ast.hpp"
#include "util/util.hpp"
#include "value.hpp"

class Runtime {
  public:
    Runtime();
    LiteralValue *get_variable_value(const std::string &name);
    void var_define(const std::string &name, LiteralValue *value);
    bool var_exists(const std::string &name);

    FunctionDefExpr *get_func_value(const std::string &name);
    void func_define(const std::string &name, FunctionDefExpr *function);
    bool func_exists(const std::string &name);

  private:
    std::unordered_map<std::string, LiteralValue *> variables;
    std::unordered_map<std::string, FunctionDefExpr *> functions;
};

class LocalRuntime : public Runtime {};

#endif // RUNTIME_RUNTIME_HPP
