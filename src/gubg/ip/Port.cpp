#include <gubg/ip/Port.hpp>

#include "gubg/platform.h"
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace gubg { namespace ip { 

    void Port::from_hbo(std::uint16_t port_hbo)
    {
        hbo_ = port_hbo;
    }
    std::uint16_t Port::as_hbo() const
    {
        return hbo_;
    }

    void Port::from_nbo(std::uint16_t port_nbo)
    {
        hbo_ = ntohs(port_nbo);
    }
    std::uint16_t Port::as_nbo() const
    {
        return htons(hbo_);
    }

    bool Port::operator==(const Port &rhs) const
    {
        return hbo_ == rhs.hbo_;
    }

    void Port::stream(std::ostream &os) const
    {
        os << "[Port](" << as_hbo() << ")";
    }

} } 
