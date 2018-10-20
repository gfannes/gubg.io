#ifndef HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED

#include "gubg/ip/Codes.hpp"
#include "gubg/ip/Types.hpp"
#include "gubg/ip/Address.hpp"
#include <ostream>

namespace gubg { namespace ip { 

    class Socket
    {
    public:
        Socket(){}
        Socket(Type, Version);
        Socket(Socket &&);
        ~Socket();
        Socket &operator=(Socket &&);

        bool setup(Type, Version);

        void close();

        bool valid() const;

        ReturnCode bind(const Address &, Port);

        void stream(std::ostream &os) const;

    private:
        int descriptor_ = -1;
        Type type_ = Type::UDP;
        Version version_ = Version::V4;
    };

    inline std::ostream &operator<<(std::ostream &os, const Socket &socket)
    {
        socket.stream(os);
        return os;
    }

} } 

#endif
