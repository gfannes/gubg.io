#ifndef HEADER_gubg_ip_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Types_hpp_ALREADY_INCLUDED

#include <cstdint>

namespace gubg { namespace ip { 

    enum class Version {V4, V6};

    enum class Type {UDP, TCP};

    using Port = std::uint16_t;

} } 

#endif
