#ifndef HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Socket_hpp_ALREADY_INCLUDED

#include <gubg/ip/Codes.hpp>
#include <gubg/ip/Types.hpp>
#include <gubg/ip/Endpoint.hpp>
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

        //Getters
        Type type() const;
        Version version() const;
        bool is_blocking() const;

        //Setters
        Socket &set_blocking(bool);

        bool create(Type, Version, int descr = -1);
        //Retains the blocking state
        void close();

        bool valid() const;

        ReturnCode reuse_address(bool);
        ReturnCode bind(const Endpoint &);
        ReturnCode listen();
        ReturnCode accept(Socket &, Endpoint &);
        ReturnCode connect(const Endpoint &);
        ReturnCode is_connected();

        //TCP send/recv
        ReturnCode send(const void *buffer, std::size_t size, std::size_t &offset);
        ReturnCode recv(void *buffer, std::size_t size, std::size_t &offset);

        //UDP send/recv
        ReturnCode sendto(unsigned int &nr_sent, const void *buffer, unsigned int size, const Endpoint &ep);
        template <typename Range>
        ReturnCode sendto(Range &data, const Endpoint &ep)
        {
            static_assert(sizeof(typename Range::value_type) == 1, "Socket::sendto() can only work on byte-like things");

            unsigned int nr_sent = 0;
            const void *buffer = &data.front();
            const auto rc = sendto(nr_sent, buffer, data.size(), ep);
            if (rc == ReturnCode::OK)
                data.pop_front(nr_sent);
            return rc;
        }
        ReturnCode recvfrom(unsigned int &nr_received, void *buffer, unsigned int size, Endpoint &ep);
        template <typename String>
        ReturnCode recvfrom(String &data, Endpoint &ep)
        {
            static_assert(sizeof(typename String::value_type) == 1, "Socket::recvfrom() can only work on byte-like things");

            unsigned int nr_received = 0;
            void *buffer = &data.front();
            const auto rc = recvfrom(nr_received, buffer, data.size(), ep);
            switch (rc)
            {
                case ReturnCode::OK:
                case ReturnCode::WouldBlock:
                    data.resize(nr_received);
                    break;
            }
            return rc;
        }

        void stream(std::ostream &os) const;

    private:
        int descriptor_ = -1;

        Type type_ = Type::UDP;
        Version version_ = Version::V4;
        bool is_blocking_ = true;
    };

    inline std::ostream &operator<<(std::ostream &os, const Socket &socket)
    {
        socket.stream(os);
        return os;
    }

} } 

#endif
