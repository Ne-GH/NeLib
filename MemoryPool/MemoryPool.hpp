//
// Created by yongheng on 23-5-23.
//

#ifndef MEMORYPOOL_MEMORYPOOL_HPP
#define MEMORYPOOL_MEMORYPOOL_HPP
#include <iostream>
struct BitMapNode;

struct ListNode {
    BitMapNode *address = nullptr;
    ListNode *next = nullptr;
    ListNode *pre = nullptr;
};
struct BitMapNode {
    bool flag = true;
    ListNode *address = nullptr;
};
static ListNode* CreateList(ListNode &pre, size_t level, size_t len) {
    if (level > len) {
        return &pre;
    }
    auto cur = new ListNode;
    pre.next = cur;
    cur->pre = &pre;
    CreateList(*cur,level + 1,len);
}
class MemoryPool {
	size_t _block_size = 128;
	size_t _pool_size = 10240;
    size_t _bit_map_size = 0;
    BitMapNode * _bit_map = nullptr;
    ListNode *_free_node = nullptr;
    ListNode *_alloc_node = nullptr;
    void *_pool_address = nullptr;


	MemoryPool(size_t memory_pool_size = 10240,size_t memory_block_size = 4) {
		_pool_size = memory_pool_size;
        _block_size = memory_pool_size;
        _pool_address = malloc(_pool_size);
        _bit_map_size = _pool_size / _block_size;
        _bit_map = (BitMapNode *)calloc(_bit_map_size,sizeof(BitMapNode));


        // 构造list,此处实际构造的链表长度为_bitmap_size + 1
        // free_node -> ... -> alloc_node -> nullptr
        _free_node = (ListNode *)malloc(sizeof(ListNode));
        _alloc_node = CreateList(*_free_node,2,_bit_map_size);

        // 构造bitmap 和 list 的映射关系
        auto &tmp = _free_node;
        for (int i = 0;i < _bit_map_size; ++i) {
            _bit_map[i].address = tmp;
            tmp->address = &_bit_map[i];    // &(_bit_map[i])
        }
	}
	MemoryPool(const MemoryPool&) = default;
	MemoryPool& operator = (const MemoryPool&) = default;
public:
	~MemoryPool() {
	}
	static MemoryPool& GetInstance() {
		static MemoryPool *instance = nullptr;
		if (instance == nullptr) {
			instance = (MemoryPool *)malloc(sizeof(MemoryPool));
            new (instance) MemoryPool;
			// instance->MemoryPool::MemoryPool();//手动构造
		}
		return *instance;
	}

	void* Malloc(size_t size) {
		if (size > _block_size) {
			return malloc(size);
		}
        // 修改list
        auto tmp = _free_node;
        _alloc_node->next = tmp;
        tmp->pre = _alloc_node;
        _free_node = _free_node->next;
        _free_node->pre = nullptr;
        tmp->next = nullptr;
        // 修改bitmap
        auto bitmap_node = tmp->address;
        bitmap_node->flag = false;
        auto index = bitmap_node - _bit_map;    // 相同类型指针相减结果自动/sizeof(type)
        return (char *)_pool_address + _block_size * index;
	}
	void Delete(void *ptr) {
        // 不在内存池中
		if (ptr < _pool_address
		|| ptr >= (char *)_pool_address + _pool_size) {
			return free(ptr);
		}

        auto index = (char *)ptr - (char *)_pool_address;
        auto node = _bit_map[index].address;
        if (node->next == nullptr) {
            node->pre->next = nullptr;
        }
        else {
            node->pre->next = node->next;
            node->next->pre = node->pre;
        }
        node->next = _free_node;
        _free_node->pre = node;
        _free_node = node;

        _bit_map[index].flag = true;
	}

};


#endif //MEMORYPOOL_MEMORYPOOL_HPP
