#ifndef RUNTIME_MEMORY_HPP
#define RUNTIME_MEMORY_HPP

#include <vector>
// TODO: garbage collection
class Memory {
  public:
    Memory();
    ~Memory();

    template <typename T, typename... Args>
    T *get(Args &&...args) {
        T *ptr = new T(args...);
        pointers.push_back(static_cast<void *>(ptr));
        return ptr;
    }

  private:
    std::vector<void *> pointers;
};

#endif // RUNTIME_MEMORY_HPP
