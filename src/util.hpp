#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>

inline void assert_(bool c, const std::string &msg = "", bool critical = true) {
    if (!c) {
        std::cout << "ASSERTION FAILED: " << msg;
        if (critical) {
            std::exit(EXIT_FAILURE);
        }
    }
}

#define ASSERT(c, ...) assert_(c, __VA_ARGS__)

#define UNIMPLEMENTED()                             \
    std::cout << "UNIMPLEMENTED: " << __FUNCTION__; \
    std::exit(EXIT_FAILURE)

#endif // UTIL_HPP
