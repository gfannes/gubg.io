#ifndef HEADER_gubg_log_macros_noop_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_macros_noop_hpp_ALREADY_INCLUDED

#include "gubg/macro/variadic.h"

#define TAG_1(name)
#define TAG_2(tgt, name)
#define TAG(...) GUBG_GET_ARG_3((__VA_ARGS__, TAG_2,TAG_1))(__VA_ARGS__)

#define ATTR(variable)

#endif
