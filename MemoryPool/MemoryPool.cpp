//
// Created by yongheng on 23-5-23.
//

#include <memory>
#include "MemoryPool.h"

MemoryPool& MemoryPool::GetInstance() {
    static std::unique_ptr<MemoryPool> instance(new MemoryPool);
    return *instance;
}
