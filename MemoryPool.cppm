/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/5/28
*******************************************************************************/

module;
#include <algorithm>
#include <functional>
#include <memory>
#include <ranges>
#include <set>
#include <vector>
#include "tools.h"
export module MemoryPool;

export
NAMESPACE_BEGIN(nl)
template <typename T>
class MemoryPool {
    struct MemoryNode {
        T *addr;
        size_t count;

        bool operator < (const MemoryNode &other) const {
            return count < other.count;
        }
        bool operator==(const MemoryNode &other) const {
            return addr == other.addr;
        }
    };

    T *addr_{};
    size_t count_{};
    std::vector<bool> flag_;
    std::multiset<MemoryNode> set_;

    // 最佳适应算法进行分配内存
    T *find_malloc_addr(size_t count) {
        auto find = set_.lower_bound({nullptr,count});
        return find != set_.end() ? find->addr : nullptr;
    }
    void merge(T *addr,size_t count) {
        auto cur_index = addr - addr_;
        auto back_index = cur_index + count;
        auto front_one_index = cur_index - 1;

        bool back_need_marge = back_index < flag_.size() && !flag_[back_index];
        bool front_need_marge = front_one_index >= 0 && !flag_[front_one_index];


        // 找到前面最后一个值为flag的元素
        auto tmp_index= front_one_index;
        auto front_index = 0;
        while (tmp_index --) {
            if (flag_[tmp_index])
                break;
        }
        front_index = tmp_index ++;

        if (front_need_marge) {
            if (back_need_marge) {
                auto front_it = set_.find({addr_ + front_index,0});
                auto back_it = set_.find({addr_ + back_index,0});

                auto [addr, front_count] = *front_it;
                auto [_, back_count] = *back_it;
                set_.erase(front_it);
                set_.erase(back_it);
                set_.insert({addr, front_count + count + back_count});
            }
            else {
                auto front_it = set_.find({addr_ + front_index,0});
                auto [addr,front_count] = *front_it;

                set_.erase(front_it);
                set_.insert({addr, front_count + count});
            }

        }
        else {
            if (back_need_marge) {
                // 仅后面需要合并
                auto back_it = set_.find({addr_ + back_index,0});
                auto [addr, back_count] = *back_it;
                set_.erase(back_it);
                set_.insert({addr, back_count + count});
            }
            else {
                // 前后都不需要合并
                set_.insert({addr, count});
            }
        }
    }
    void update_memory_pool(T *addr, size_t count, bool is_malloc) {
        for (long tmp = count - 1; tmp >= 0; -- tmp)
            flag_[addr - addr_ + count] = is_malloc;

        if (is_malloc) {
            auto it = std::ranges::find_if(set_, [addr](const MemoryNode &node) {
                return node.addr == addr;
            });
            if (it == set_.end())
                throw std::runtime_error("can't find this addr");

            auto [addr_back,count_back] = *it;
            set_.erase(it);
            set_.insert({addr + count, count_back - count});
        }
        else
            merge(addr,count);


    }
public:
    explicit MemoryPool(const size_t count) : count_(count) {
        flag_.resize(count_);
        addr_ = static_cast<T *>(::malloc(count_ * sizeof(T)));
        set_.insert({addr_,count});
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
        T *ret = find_malloc_addr(1);
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
