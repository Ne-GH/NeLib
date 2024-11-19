/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2024/11/19 21:15
*******************************************************************************/

module;

#include <vector>
#include <string>
#include <functional>


#include "tools.h"
export module Param;


export
NAMESPACE_BEGIN(nl)
class Param {
    std::pmr::vector<std::string> params_;
public:
    Param(const int argc,const char *const argv[]) {
        for (int i = 0;i < argc; ++i)
            params_.emplace_back(argv[i]);
    }

    void add_param_callback(const std::initializer_list<std::string> params,const std::function<void()> callback) {
        for (const auto& param : params) {
            if (std::ranges::find(params_,param) != params_.end()) {
                callback();
                return;
            }
        }
    }

    [[nodiscard]]
    bool have_param() const {
        return params_.size() > 1;
    }
    [[nodiscard]]
    size_t count_param() const {
        return params_.size() - 1;
    }

};

NAMESPACE_END(nl)
