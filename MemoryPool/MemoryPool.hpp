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

#include <iostream>
#include <thread>
#include <chrono>


using namespace std;
class MemoryPool {
	size_t _memory_block_size = 4;
	size_t _memory_pool_size = 10240;
	void* _memory_pool_address = nullptr;

	void* _bit_map_address = nullptr;
	size_t _bit_map_size = 0;
	MemoryPool(size_t memory_size = 10240) {
		_memory_pool_size = memory_size;
		_memory_pool_address = malloc(_memory_pool_size);
		_bit_map_size = _memory_pool_size / _memory_block_size;	// 此处位图简单实现，使用byte代替bit
		_bit_map_address = calloc(_bit_map_size,sizeof(char));
	}
	MemoryPool(const MemoryPool&) = default;
	MemoryPool& operator = (const MemoryPool&) = default;
public:
	~MemoryPool() {
		free(_memory_pool_address);
		_memory_pool_address = nullptr;
		free(_bit_map_address);
		_bit_map_address = nullptr;
	}
	static MemoryPool& GetInstance() {
		static MemoryPool *instance = nullptr;
		if (instance == nullptr) {
			instance = (MemoryPool *)malloc(sizeof(MemoryPool));
			instance->MemoryPool::MemoryPool();//手动构造
		}
		return *instance;
	}

	void* Malloc(size_t size) {
		if (size > 4) {
			return malloc(size);
		}
		for (int i = 0; i < _bit_map_size; ++i) {
			// 可以分配
			if (((char*)_bit_map_address)[i] == 0) {
				*((char*)_bit_map_address + i) = 1;
				return (char *)_memory_pool_address + i;
			}
		}
		// 无可分配内存；
		return malloc(size);

	}
	void Delete(void *ptr) {
		if (ptr < _memory_pool_address 
		|| ptr >= (char *)_memory_pool_address + _memory_pool_size) {
			return free(ptr);
		}

		int index = (intptr_t)ptr - (intptr_t)_memory_pool_address;
		((char*)_bit_map_address)[index] = 0;
		return;
	}

};


//void* operator new(size_t size) {
//	return MemoryPool::GetInstance().Malloc(size);
//}
//void* operator new[](size_t size) {
//	return MemoryPool::GetInstance().Malloc(size);
//}
//void operator delete(void* ptr) {
//	return MemoryPool::GetInstance().Delete(ptr);
//}
//void operator delete[](void* ptr) {
//	return MemoryPool::GetInstance().Delete(ptr);
//}

void Test() {
	vector <int*> vec(2550);
	for (auto& p : vec) {
		p = new int;
	}

	for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
		delete *it;
	}
	return;
}

#endif //MEMORYPOOL_MEMORYPOOL_HPP
