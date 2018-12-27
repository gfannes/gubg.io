#include "gubg/macro/variadic.h"

#define TAG_1(name)
#define TAG_2(tgt, name)
#define TAG(...) GUBG_GET_ARG_3((__VA_ARGS__, TAG_2,TAG_1))(__VA_ARGS__)

#define ATTR(variable)
