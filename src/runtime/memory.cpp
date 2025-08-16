#include "memory.hpp"

Memory::Memory() {}

Memory::~Memory() {
    for (int i = 0; i < pointers.size(); ++i) {
        delete pointers[i];
    }
}
