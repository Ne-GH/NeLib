/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/5/28
*******************************************************************************/

#ifndef MEMORYPOOL_MEMORYPOOL_HPP
#define MEMORYPOOL_MEMORYPOOL_HPP
#include <iostream>

static void *_Malloc(size_t size) {
    void *ret_p = malloc(size);
    if (ret_p == nullptr) {
        throw std::bad_alloc();
    }
    return ret_p;
}


class MemoryPool {
	size_t _block_size = 4;
    size_t _block_nums = 1024;
    size_t _bit_map_len = 0;
    void *_memory_pool = nullptr;
    bool *_bit_map = nullptr;
    void *_free_stack = nullptr;
    size_t _index = 0;


	MemoryPool(size_t memory_block_size = 4,
               size_t memory_block_nums = 1024) :
               _block_size(memory_block_size),
               _block_nums(memory_block_nums) {

        _bit_map_len = memory_block_nums;
        _memory_pool = _Malloc(_block_nums * _block_size);
        _bit_map = (bool *)_Malloc(sizeof(bool) * _bit_map_len);
        std::fill(_bit_map,_bit_map + _bit_map_len,true);
        _free_stack = _Malloc(sizeof(void *) * _bit_map_len);
        std::fill((char *)_free_stack,((char *)_free_stack + _bit_map_len), (unsigned)0);

        for (int i = 0;i < _block_nums; ++i) {
            ((char **)_free_stack)[i] = (char *)_memory_pool + i;
        }
        _index = _block_nums - 1;

	}
	MemoryPool(const MemoryPool&) = default;
	MemoryPool& operator = (const MemoryPool&) = default;
public:
	~MemoryPool() {
        delete[] _bit_map;
        _bit_map = nullptr;
        free(_memory_pool);
        _memory_pool = nullptr;
        free(_free_stack);
        _free_stack = nullptr;
	}
	static MemoryPool& GetInstance() {
		static MemoryPool *instance = nullptr;
		if (instance == nullptr) {
			instance = (MemoryPool *)malloc(sizeof(MemoryPool));
            new (instance) MemoryPool(8,10240);
		}
		return *instance;
	}

    void* Malloc(size_t size) {
        if (_index > _block_nums || size > _block_size) {
            return malloc(size);
        }

        void * ret = (void *)*((intptr_t *)_free_stack + _index);
        _bit_map[_index] = false;
        *((intptr_t **)_free_stack + _index) = nullptr;
        _index --;
        return ret;
    }
	void Delete(void *ptr) {
        if (ptr < _memory_pool || ptr > (char *)_memory_pool + _block_size * _block_nums) {
            free (ptr);
        }

        int index = ((char *)ptr - (char *)_memory_pool) / _block_size;
        _bit_map[index] = true;
        *((intptr_t **)_free_stack + _index++) = (intptr_t*)ptr;
	}

};


#endif //MEMORYPOOL_MEMORYPOOL_HPP
