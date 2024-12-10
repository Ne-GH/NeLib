/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/12/10 22:32
*******************************************************************************/

module;
#include <memory>


#include "tools.h"
export module Matrix;

import MultArray;

export
NAMESPACE_BEGIN(nl)

class Matrix {
    size_t col_{}, row_{};
    std::shared_ptr<double[]> data_ptr_;
    nl::MultArray<double> data_;

public:
    Matrix() { }
    Matrix(size_t col, size_t row) :
        data_ptr_(std::make_shared<double[]>(col * row) ),
        data_(data_ptr_.get(), {row, col }) {  }

    Matrix operator + (const Matrix & right) const {
        return {};
    }
    Matrix operator - (const Matrix & right) const {
        return {};
    }
    Matrix operator * (const Matrix & right) const {
        return {};
    }
    Matrix operator / (const Matrix & right) const {
        return {};
    }
    Matrix& operator += (const Matrix & right) {
        return *this;
    }
    Matrix& operator -= (const Matrix & right) {
        return *this;
    }
    Matrix& operator *= (const Matrix & right) {
        return *this;
    }
    Matrix& operator /= (const Matrix & right) {
        return *this;
    }
};

NAMESPACE_END(nl)
