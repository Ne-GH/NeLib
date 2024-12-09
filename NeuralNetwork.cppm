/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/12/06 22:51
*******************************************************************************/

module;
#include <array>

#include "tools.h"
export module NeuralNetwork;

export
NAMESPACE_BEGIN(nl)

template<std::size_t ...Args>
class NeuralNetwork {
    std::array<double, sizeof...(Args)> weights;
};
/*******************************************************************************

NeuralNetwork<2,3,5,1>

*******************************************************************************/



NAMESPACE_END(nl)

