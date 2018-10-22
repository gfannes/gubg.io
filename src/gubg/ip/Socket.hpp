#ifndef HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED

#include "gubg/ip/Codes.hpp"
#include "gubg/ip/Types.hpp"
#include "gubg/ip/Endpoint.hpp"
#include <ostream>
#include <string>
#include <cstddef>

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

        ReturnCode get_blocking(bool &block) const;
        ReturnCode set_blocking(bool block);

        ReturnCode bind(const Endpoint &);

        ReturnCode sendto(unsigned int &sent, const void *buffer, unsigned int size, const Endpoint &ep);

        template <typename Range>
        ReturnCode sendto(Range &data, const Endpoint &ep)
        {
            static_assert(sizeof(typename Range::value_type) == 1, "Socket::sendto() can only work on byte-like things");

            unsigned int sent = 0;
            const void *buffer = &data.front();
            const auto rc = sendto(sent, buffer, data.size(), ep);
            if (rc == ReturnCode::OK)
                data.pop_front(sent);
            return rc;
        }

        ReturnCode recvfrom(unsigned int &recv, void *buffer, unsigned int size, Endpoint &ep);

        template <typename String>
        ReturnCode recvfrom(String &data, Endpoint &ep)
        {
            static_assert(sizeof(typename String::value_type) == 1, "Socket::recvfrom() can only work on byte-like things");

            unsigned int recv = 0;
            void *buffer = &data.front();
            const auto rc = recvfrom(recv, buffer, data.size(), ep);
            switch (rc)
            {
                case ReturnCode::OK:
                case ReturnCode::WouldBlock:
                    data.resize(recv);
                    break;
            }
            return rc;
        }

        void stream(std::ostream &os) const;

    private:
        int descriptor_ = -1;
        bool is_blocking_ = true;
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
