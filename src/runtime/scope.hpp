#ifndef RUNTIME_SCOPE_HPP
#define RUNTIME_SCOPE_HPP

#include "runtime/runtime.hpp"

struct Scope {
    Scope();

    Runtime runtime;
    Scope *parent = nullptr; // nullptr means it is global root
};

#endif // RUNTIME_SCOPE_HPP
