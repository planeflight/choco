#include "memory.hpp"

Memory::Memory() {}

Memory::~Memory() {
    while (pointers.size() > 0) {
        delete pointers.back();
        pointers.pop_back();
    }
}
