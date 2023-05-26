#include <iostream>
#include <chrono>
#include "MemoryPool.hpp"
using namespace std;
class T {
public:
    T() {
        std::cout << "构造函数" << std::endl;
    }
    ~T() {
        std::cout << "析构函数" << std::endl;
    }
};

void* operator new(size_t size) {
	return MemoryPool::GetInstance().Malloc(size);
}
void* operator new[](size_t size) {
	return MemoryPool::GetInstance().Malloc(size);
}
void operator delete(void* ptr) {
	return MemoryPool::GetInstance().Delete(ptr);
}
void operator delete[](void* ptr) {
	return MemoryPool::GetInstance().Delete(ptr);
}
int main() {

    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();

    start = chrono::system_clock::now();
    auto p = new int;
    delete p;
    end = chrono::system_clock::now();
    std::cout << chrono::duration<double,micro>(end - start) << std::endl;


//    auto p = new T[3];
//
//    delete[] p;

    return 0;
}
