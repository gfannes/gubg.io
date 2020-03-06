#include "gubg/ip/Endpoint.hpp"
#include <cstring>

#include "gubg/platform.h"
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netdb.h>
#endif

namespace gubg { namespace ip { 

    Endpoint::Endpoint()
    {
        setup(Address{}, Port{});
    }

    Endpoint::Endpoint(const Address &address)
    {
        setup(address, Port{});
    }
    Endpoint::Endpoint(const Address &address, std::uint16_t port_hbo)
    {
        Port port;
        port.from_hbo(port_hbo);
        setup(address, port);
    }
    Endpoint::Endpoint(const Address &address, const Port &port)
    {
        setup(address, port);
    }

    bool Endpoint::valid() const
    {
        return address_.valid() && port_.valid();
    }

    void Endpoint::setup(const Address &address, const Port &port)
    {
        address_ = address;
        port_ = port;

        std::memset(&sa_, 0, sizeof(sa_));

        switch (address.version())
        {
            case Version::V4:
                {
                    auto &sin = *(struct sockaddr_in *)(&sa_);
                    sin.sin_family = AF_INET;
                    sin.sin_port = port.as_nbo();
                    sin.sin_addr.s_addr = address.as_nbo();
                }
                break;
        }
    }

    void Endpoint::stream(std::ostream &os) const
    {
        os << address_ << std::endl;
        os << port_ << std::endl;
    }

} } 
