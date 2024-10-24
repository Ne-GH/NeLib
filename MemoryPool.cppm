/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/5/28
*******************************************************************************/

module;

#include <iostream>
#include "tools.h"
export module MemoryPool;

static void *Malloc(size_t size) {
    void *ret_p = malloc(size);
    if (ret_p == nullptr) {
        throw std::bad_alloc();
    }
    return ret_p;
}

NAMESPACE_BEGIN(nl)


class MemoryPool {
	size_t block_size_ = 4;
    size_t block_nums_ = 1024;
    size_t bit_map_len_ = 0;
    void *memory_pool_ = nullptr;
    bool *bit_map_ = nullptr;
    void *free_stack_ = nullptr;
    size_t index_ = 0;


	MemoryPool(size_t memoryblock_size_ = 4,
               size_t memoryblock_nums_ = 1024) :
               block_size_(memoryblock_size_),
               block_nums_(memoryblock_nums_) {

        bit_map_len_ = memoryblock_nums_;
        memory_pool_ = Malloc(block_nums_ * block_size_);
        bit_map_ = (bool *)Malloc(sizeof(bool) * bit_map_len_);
        std::fill(bit_map_,bit_map_ + bit_map_len_,true);
        free_stack_ = Malloc(sizeof(void *) * bit_map_len_);
        std::fill((char *)free_stack_,((char *)free_stack_ + bit_map_len_), (unsigned)0);

        for (int i = 0;i < block_nums_; ++i) {
            ((char **)free_stack_)[i] = (char *)memory_pool_ + i;
        }
        index_ = block_nums_ - 1;

	}
	MemoryPool(const MemoryPool&) = default;
	MemoryPool& operator = (const MemoryPool&) = default;
public:
	~MemoryPool() {
        delete[] bit_map_;
        bit_map_ = nullptr;
        free(memory_pool_);
        memory_pool_ = nullptr;
        free(free_stack_);
        free_stack_ = nullptr;
	}
	static MemoryPool& GetInstance() {
		static MemoryPool *instance = nullptr;
		if (instance == nullptr) {
			instance = (MemoryPool *)::malloc(sizeof(MemoryPool));
            new (instance) MemoryPool(8,10240);
		}
		return *instance;
	}

    void* malloc(size_t size) {
        if (index_ > block_nums_ || size > block_size_) {
            return malloc(size);
        }

        void * ret = (void *)*((intptr_t *)free_stack_ + index_);
        bit_map_[index_] = false;
        *((intptr_t **)free_stack_ + index_) = nullptr;
        index_ --;
        return ret;
    }
	void del(void *ptr) {
        if (ptr < memory_pool_ || ptr > (char *)memory_pool_ + block_size_ * block_nums_) {
            free (ptr);
        }

        int index = ((char *)ptr - (char *)memory_pool_) / block_size_;
        bit_map_[index] = true;
        *((intptr_t **)free_stack_ + index_++) = (intptr_t*)ptr;
	}

};


NAMESPACE_END(nl)
