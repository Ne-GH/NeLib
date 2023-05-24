//
// Created by yongheng on 23-5-23.
//

#ifndef MEMORYPOOL_MEMORYPOOL_HPP
#define MEMORYPOOL_MEMORYPOOL_HPP
#include <iostream>
#include <map>
class MemoryPool {
    char *memory_pool_address = nullptr;
    std::map<size_t ,void*> _free_map;
    std::map<void* ,size_t> _malloc_map;
    size_t memory_pool_size = 10240;
public:
    MemoryPool() {
        _free_map[memory_pool_size] = new char[memory_pool_size];
    }
    ~MemoryPool() {
        delete[] memory_pool_address;
        memory_pool_address = nullptr;
    }

    static MemoryPool& GetInstance() {

    }

    void *Malloc(size_t size) {

    }
    void Delete(void *) {

    }



};

void *operator new(size_t size) {
    return MemoryPool::GetInstance().Malloc(size);
}
void *operator new[](size_t size) {
    return MemoryPool::GetInstance().Malloc(size);
}
void operator delete(void *ptr) noexcept {
    return MemoryPool::GetInstance().Delete(ptr);
}
void operator delete[](void *ptr) noexcept {
    return MemoryPool::GetInstance().Delete(ptr);
}

#endif //MEMORYPOOL_MEMORYPOOL_HPP
