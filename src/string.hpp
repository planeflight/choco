#ifndef STRING_HPP
#define STRING_HPP

#include <cstring>

inline bool compare(const char *s1, size_t n1, const char *s2, size_t n2) {
    if (n1 != n2) return false;

    for (size_t i = 0; i < n1; ++i) {
        if (s1[i] != s2[i]) return false;
    }
    return true;
}

#endif // STRING_HPP
