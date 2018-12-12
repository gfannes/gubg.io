#ifndef HEADER_gubg_log_macros_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_macros_hpp_ALREADY_INCLUDED

#include "gubg/log/Scope.hpp"

#define TAG(name) gubg::log::Scope l_gubg_log_Scope{name};

#define ATTR(variable) l_gubg_log_Scope.attr(#variable, variable);

#endif
