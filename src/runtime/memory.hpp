#ifndef RUNTIME_MEMORY_HPP
#define RUNTIME_MEMORY_HPP

#include <vector>

#include "runtime/value.hpp"

// TODO: garbage collection
class Memory {
  public:
    Memory();
    ~Memory();

    template <typename T, typename... Args>
    T *get(Args &&...args) {
        // Compile-time sanity check
        static_assert(std::is_base_of<LiteralValue, T>::value,
                      "T not derived from LiteralValue");
        T *ptr = new T(args...);
        pointers.push_back(ptr);
        return ptr;
    }

  private:
    std::vector<LiteralValue *> pointers;
};

#endif // RUNTIME_MEMORY_HPP
