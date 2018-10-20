#ifndef HEADER_gubg_ip_resolve_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_resolve_hpp_ALREADY_INCLUDED

#include "gubg/ip/Codes.hpp"
#include "gubg/ip/Address.hpp"
#include <string>

namespace gubg { namespace ip { 

    ReturnCode resolve(Address &, const std::string &hostname);

} } 



#endif
