#ifndef RUNTIME_RUNTIME_HPP
#define RUNTIME_RUNTIME_HPP

#include <string>
#include <unordered_map>

#include "value.hpp"

class Runtime {
  public:
    Runtime();
    LiteralValue *get_variable_value(const std::string &name);
    void define(const std::string &name, LiteralValue *value);
    bool exists(const std::string &name);

  private:
    std::unordered_map<std::string, LiteralValue *> variables;
};

class LocalRuntime : public Runtime {};

#endif // RUNTIME_RUNTIME_HPP
