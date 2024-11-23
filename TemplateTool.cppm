module;
#include <type_traits>


#include "tools.h"
export module TemplateTools;

export
NAMESPACE_BEGIN(nl)

template<typename T>
struct is_void {
    static constexpr  bool value = false;
};
template<>
struct is_void<void> {
    static constexpr  bool value = true;
};

template <typename T,typename U>
struct is_same {
    static constexpr  bool value = false;
};
template<typename T>
struct is_same<T,T> {
    static constexpr  bool value = true;
};
template<typename T,typename U>
constexpr bool is_same_v = is_same<T,U>::value;


template <typename T>
constexpr T &&forward(std::remove_reference_t<T> &val) {
    return static_cast<T &&>(val);
}


NAMESPACE_END(nl)
