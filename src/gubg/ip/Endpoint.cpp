#include "gubg/ip/Endpoint.hpp"
#include <cstring>

#include <sys/types.h>
#include <netdb.h>

namespace gubg { namespace ip { 

    Endpoint::Endpoint()
    {
        setup_(Address{}, 0);
    }

    Endpoint::Endpoint(const Address &address)
    {
        setup_(address, 0);
    }
    Endpoint::Endpoint(const Address &address, Port port)
    {
        setup_(address, port);
    }

    void Endpoint::setup_(const Address &address, Port port)
    {
        std::memset(&sa_, 0, sizeof(sa_));

        switch (address.version())
        {
            case Version::V4:
                {
                    auto &sin = *(struct sockaddr_in *)(&sa_);
                    sin.sin_family = AF_INET;
                    sin.sin_port = port;
                    sin.sin_addr.s_addr = address.as_uint32();
                }
                break;
        }
    }

} } 
