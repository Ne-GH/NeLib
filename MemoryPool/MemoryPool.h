//
// Created by yongheng on 23-5-23.
//

#ifndef MEMORYPOOL_MEMORYPOOL_H
#define MEMORYPOOL_MEMORYPOOL_H
#include <iostream>
#include <map>
class MemoryPool {

    static MemoryPool& GetInstance();
};

inline void *operator new(size_t size) {
    return malloc(size);
}
inline void *operator new[](size_t size) {
    std::cout << size << std::endl;
    return malloc(size);
}
inline void operator delete(void *ptr) noexcept {
    free(ptr);
}
inline void operator delete[](void *ptr) noexcept {
    free(ptr);
}
#endif //MEMORYPOOL_MEMORYPOOL_H
