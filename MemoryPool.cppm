/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/5/28
*******************************************************************************/

module;
#include <memory>
#include <vector>
#include "tools.h"
export module MemoryPool;


/*******************************************************************************
 * 1、需要分配一大块内存用于存储T类型对象
 * 2、分配一个对象之后需要返回指向该对象的指针，使得能够索引到该对象，至于析构后该对象不存在，暂不考虑
 * 3、释放指定对象
*******************************************************************************/
NAMESPACE_BEGIN(nl)
template <typename T>
class MemoryPool {
    T *addr_{};
    size_t count_{};
    std::vector<bool> flag_;

    // 采用最早适应算法进行分配
    T *find_malloc_addr(size_t count) {

        return{};
    }
    void update_memory_pool(T *addr, size_t count, bool is_malloc) {
        while (count--)
            flag_[addr - addr_] = is_malloc;
    }
public:
    explicit MemoryPool(const size_t count) : count_(count){
        flag_.resize(count_);
        addr_ = ::malloc(count_ * sizeof(T));
    }
    ~MemoryPool() {
        delete addr_;
        addr_ = nullptr;
    }
    static std::unique_ptr<MemoryPool>& GetInstance() {
        static std::unique_ptr<MemoryPool> instance;
        if (!instance)
            instance = std::make_unique<MemoryPool>();
        return instance;
    }

    T *malloc() {
        auto ret = find_malloc_addr(1);
        if (ret == nullptr)
            throw std::bad_alloc();
        ::new (ret) T;
        update_memory_pool(ret,1,true);
        return ret;
    }
    // free中应调用 delete
    void free(T *addr) {
        delete addr;
        update_memory_pool(addr,1,false);
        return;
    }

    std::tuple<T *,size_t> malloc(size_t count) {
        auto ret = find_malloc_addr(count);

        for (int i = 0;i < count; ++i)
            new (ret + i) T();

        update_memory_pool(ret, count, true);
        return {ret, count};
    }
    void free(std::tuple<T *,size_t> addr_info) {
        auto [addr, count] = addr_info;

        for (int i = 0;i < count; ++i)
            delete (addr + i);

        update_memory_pool(addr, count, false);
    }
};
NAMESPACE_END(nl)
