#ifndef HEADER_gubg_log_macros_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_macros_hpp_ALREADY_INCLUDED

#include "gubg/log/Scope.hpp"
#include "gubg/macro/variadic.h"

#define TAG_1(name)     gubg::log::Scope l_gubg_log_Scope{name};
#define TAG_2(tgt, name) gubg::log::Scope l_gubg_log_Scope{tgt, name};
#define TAG(...) GUBG_GET_ARG_3((__VA_ARGS__, TAG_2,TAG_1))(__VA_ARGS__)

#define ATTR(variable) l_gubg_log_Scope.attr(#variable, variable);

#endif
