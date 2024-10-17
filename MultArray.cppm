/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/10/16 22:59
*******************************************************************************/

module;
#include <utility>
#include <vector>
#include <span>
#include "tools.h"
export module MultArray;

export
NAMESPACE_BEGIN(nl)


template<typename T>
class MultArray {
    T* data_;
    size_t count_;
    std::vector<int> dimensions_;
public:
    MultArray(T* p, std::vector<int> dims) : data_(p), dimensions_(std::move(dims)) {
        count_ = 1;
        for (const int val : dimensions_)
            count_ *= val;
    }

    T& operator()(const std::vector<int> &indices) {
        auto index = 0;
        for (size_t i = 0; i < indices.size(); ++i)
            index = index * dimensions_[i] + indices[i];
        return data_[index];
    }

    [[nodiscard]]
    size_t size() const {
        return dimensions_[0];
    }

    MultArray operator[] (const size_t index) {
        return MultArray(data_ + index * count_ / dimensions_[0],std::vector(dimensions_.begin() + 1,dimensions_.end()));
    }

    std::span<T> to_span() {
        return std::span<T>(data_, count_);
    }

};

NAMESPACE_END(nl)