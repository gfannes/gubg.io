#ifndef HEADER_gubg_ip_Endpoint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Endpoint_hpp_ALREADY_INCLUDED

#include "gubg/ip/Address.hpp"

#include "gubg/platform.h"
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace gubg { namespace ip { 

    class Endpoint
    {
    public:
        Endpoint();
        Endpoint(const Address &);
        Endpoint(const Address &, Port);

        struct sockaddr &as_sockaddr() {return sa_;}
        const struct sockaddr &as_sockaddr() const {return sa_;}

    private:
        void setup_(const Address &, Port);

        struct sockaddr sa_;
    };

} } 

#endif
