/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/12/06 22:51
*******************************************************************************/

module;
#include <array>
#include <tuple>
#include "tools.h"
export module NeuralNetwork;

template <std::size_t... N>
struct NeuralNetworkHelper;

template <std::size_t Row, std::size_t Col, std::size_t... Args>
struct NeuralNetworkHelper<Row, Col, Args...> {
    using type = std::remove_cvref_t<decltype(std::tuple_cat(
                        std::declval<std::tuple<std::array<std::array<double,Col>, Row>>>(),
                        std::declval<typename NeuralNetworkHelper<Col, Args...>::type>()
                    ))>;
};

template <std::size_t Last>
struct NeuralNetworkHelper<Last> {
    using type = std::tuple<>;
};

export
NAMESPACE_BEGIN(nl)

template <std::size_t... N>
class NeuralNetwork {
    typename NeuralNetworkHelper<N ...>::type weights_;
    std::tuple<std::array<double, N> ...> inputs_;
    std::tuple<std::array<double, N> ...> outputs_;
    std::size_t layout_count_ = sizeof ... (N);

public:
    NeuralNetwork() = default;
};

NAMESPACE_END(nl)

