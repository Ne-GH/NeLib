/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/12/06 22:51
*******************************************************************************/

module;
#include <array>
#include <tuple>
#include <iostream>
#include <type_traits>
#include <vector>

#include "tools.h"
export module NeuralNetwork;


export
NAMESPACE_BEGIN(nl)

/*******************************************************************************
NeuralNetwork nn({1,2,3,4})
*******************************************************************************/
/*
template <typename ... Args>
class NeuralNetwork {
    std::vector<std::vector<double>> weights_;


    template <size_t ... index>
    void neural_network_impl(auto &&tuple, std::index_sequence<index...>) {
        (weights_[index].resize(std::get<index>(tuple)), ...);
    }

public:
    NeuralNetwork(std::vector<size_t> weights) {
        weights_.resize(weights.size());
        for (int i = 0; i < weights.size(); i++) {
            weights_[i].resize(weights[i]);
        }
    }
    NeuralNetwork(Args && ... args) {
        weights_.resize(sizeof...(args));
        auto tuple = std::forward_as_tuple(args...);
        auto callback = std::get<sizeof ...(args) - 1>(tuple);
        neural_network_impl(tuple, std::make_index_sequence<sizeof...(args) - 1>());

    }
};
*/

// 定义矩阵类型
template <typename T, std::size_t Rows, std::size_t Cols>
using Matrix = std::array<std::array<T, Cols>, Rows>;

// 递归地从模板参数包创建矩阵元组
template <typename T, std::size_t... N>
struct NeuralNetworkHelper;

template <typename T, std::size_t First, std::size_t Second, std::size_t... Rest>
struct NeuralNetworkHelper<T, First, Second, Rest...> {
    using type = decltype(std::tuple_cat(
        std::tuple<Matrix<T, First, Second>>{},  // 当前矩阵
        typename NeuralNetworkHelper<T, Second, Rest...>::type{}  // 递归下一组
    ));
};

template <typename T, std::size_t Last>
struct NeuralNetworkHelper<T, Last> {
    using type = std::tuple<>;  // 递归结束时返回空元组
};

template <typename T, std::size_t... N>
using NeuralNetworkMatrices = typename NeuralNetworkHelper<T, N...>::type;

// 神经网络类
template <std::size_t... N>
class NeuralNetwork {
    using Matrices = NeuralNetworkMatrices<double, N...>;  // 自动生成矩阵类型
    Matrices weights_;  // 保存所有矩阵

    // 遍历并打印元组中的所有矩阵
    template <std::size_t Index = 0>
    void print_matrices() const {
        if constexpr (Index < std::tuple_size<Matrices>::value) {
            const auto& matrix = std::get<Index>(weights_);
            std::cout << "Matrix " << Index + 1 << ":\n";
            for (const auto& row : matrix) {
                for (const auto& elem : row) {
                    std::cout << elem << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
            print_matrices<Index + 1>();
        }
    }

public:
    NeuralNetwork() {
        initialize_matrices();
        print_matrices();
    }

    // 初始化所有矩阵
    template <std::size_t Index = 0>
    void initialize_matrices() {
        if constexpr (Index < std::tuple_size<Matrices>::value) {
            auto& matrix = std::get<Index>(weights_);
            for (std::size_t i = 0; i < matrix.size(); ++i) {
                for (std::size_t j = 0; j < matrix[i].size(); ++j) {
                    matrix[i][j] = Index + 1;  // 示例值填充：第一个矩阵用1填充，第二个用2，依此类推
                }
            }
            initialize_matrices<Index + 1>();
        }
    }
};


int main() {

    // 2 3 4 5
    // 3 4 5
    NeuralNetwork<2,3,4,5> neural_network;



    return 0;
}




NAMESPACE_END(nl)

