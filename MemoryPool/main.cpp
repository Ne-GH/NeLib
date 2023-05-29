#include <iostream>
#include <chrono>
#include "MemoryPool.hpp"
using namespace std;
class T {
    int num;
    int val;
public:
    T() {
//        std::cout << "构造函数" << std::endl;
    }
    ~T() {
//        std::cout << "析构函数" << std::endl;
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
#define NUMS 10240
void use_new() {
    T *arr[NUMS] = {0};
    for(int i = 0;i < NUMS; ++i) {
        arr[i] = new T;
    }
    for(int i = 0;i < NUMS; ++i) {
        delete arr[i];
    }
}
//void use_pool() {
//    int *arr[NUMS] = {0};
//    for(int i = 0;i < NUMS; ++i) {
//        arr[i] = (int *)MemoryPool::GetInstance().Malloc(sizeof(int));
//    }
//    for(int i = 0;i < 1024; ++i) {
//        MemoryPool::GetInstance().Delete(arr[i]);
//    }
//}
int main() {

    cout << sizeof(T) << endl;
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();

    start = chrono::system_clock::now();
    use_new();
    end = chrono::system_clock::now();
    std::cout << chrono::duration<double,micro>(end - start) << std::endl;

    return 0;
}
