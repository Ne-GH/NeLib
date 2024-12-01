/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/12/01 22:56
*******************************************************************************/


module;
#include <array>
#include <opencv2/core/base.hpp>

#include "tools.h"
export module AverageQueue;


NAMESPACE_BEGIN(nl)
template <typename T,size_t max_count>
class AverageQueue {
    std::array<T,max_count> data;
    int cur_count{};
public:
    AverageQueue(std::initializer_list<T> init_list) : cur_count(init_list.size()) {
        if (init_list.size() > max_count)
            throw std::invalid_argument("AverageQueue is too many elements");

        std::copy(init_list.begin(), init_list.end(), data.begin());
    }
    T operator() () {
        T ret{};
        for (int i = 0;i < cur_count; ++i)
            ret += data[i];
        return ret / cur_count;
    }
    AverageQueue &push_back(T value) {


    }



};


NAMESPACE_END(nl)
