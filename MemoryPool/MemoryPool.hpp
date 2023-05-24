//
// Created by yongheng on 23-5-23.
//

#ifndef MEMORYPOOL_MEMORYPOOL_HPP
#define MEMORYPOOL_MEMORYPOOL_HPP
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>

class MemoryPool {
    size_t _memory_pool_size;
    void *_memory_pool_address;
    std::multimap<size_t,void *> _free_map;
    std::multimap<void* ,size_t> _alloc_map;


    MemoryPool(const MemoryPool &) = default;
    MemoryPool& operator = (const MemoryPool &) = default;
public:
    MemoryPool(size_t memory_pool_size = 10240) {
        std::cout << "1" << std::endl;      // debug
        _memory_pool_size = memory_pool_size;
        _memory_pool_address = (void *)malloc(_memory_pool_size);
        // _memory_pool_address = new char[_memory_pool_size];      // 改用malloc
        _free_map.insert({_memory_pool_size,_memory_pool_address});
    }
    ~MemoryPool() {
        free(_memory_pool_address);
        _memory_pool_address = nullptr;
    }
    static MemoryPool& GetInstance() {
        static std::unique_ptr<MemoryPool> _instance = std::make_unique<MemoryPool>();
        return *_instance;
    }

    void *Malloc(size_t size) {
        auto it = _free_map.upper_bound(size);
        // 没有空闲内存,调用malloc
        if (it == _free_map.end()) {
            return malloc(size);
        }
        // 找到适合的内存
        _free_map.insert({it->first - size,(char *)(it->second) + size});
        _alloc_map.insert({it->second,size});
        auto ret_address = it->second;
        it = _free_map.erase(it);
        return ret_address;
    }

    void Delete(void *ptr) {
        auto it = _alloc_map.find(ptr);
        // 不在内存池中
        if (it == _alloc_map.end()) {
            free(ptr);
        }
        // 在内存池中
        auto del_it = _free_map.insert({it->second,it->first});
        _alloc_map.erase(it);

        // 合并
        std::vector<decltype(_free_map.end())> vec;
        for (auto tmp = _free_map.begin();tmp != _free_map.end(); ++tmp) {
            // 低地址相邻
            if ((char *)tmp->second + tmp->first == del_it->second) {
                vec.push_back(tmp);
                break;
            }
            // 高地址相邻
            if ((char *)del_it->second + del_it->first == tmp->second) {
                vec.push_back(tmp);
                break;
            }
        }
        vec.push_back(del_it);
        std::sort(vec.begin(),vec.end(),[&](decltype(_free_map.end()) a, decltype(_free_map.end()) b) -> bool {
            if (a->second < b->second)
                return true;
            return false;
        });
        auto start_address = vec[0]->second;
        size_t new_size = 0;
        for (auto tmp : vec) {
            new_size += tmp->first;
            _free_map.erase(tmp);
        }
        _free_map.insert({new_size,start_address});

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
