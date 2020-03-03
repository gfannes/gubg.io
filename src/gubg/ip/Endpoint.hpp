#ifndef HEADER_gubg_ip_Endpoint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Endpoint_hpp_ALREADY_INCLUDED

#include <gubg/ip/Address.hpp>
#include <gubg/ip/Port.hpp>

#include <gubg/platform.h>
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
        Endpoint(const Address &, std::uint16_t port_hbo);
        Endpoint(const Address &, const Port &);

        static Endpoint localhost(std::uint16_t port_hbo) { return Endpoint(Address{127,0,0,1}, port_hbo); }

        void setup(const Address &, const Port &);

        struct sockaddr &as_sockaddr() {return sa_;}
        const struct sockaddr &as_sockaddr() const {return sa_;}

        void stream(std::ostream &os) const;

    private:
        Address address_;
        Port port_;
        struct sockaddr sa_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Endpoint &ep)
    {
        ep.stream(os);
        return os;
    }

} } 

#endif
