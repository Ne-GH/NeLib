module;
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


NAMESPACE_END(nl)
