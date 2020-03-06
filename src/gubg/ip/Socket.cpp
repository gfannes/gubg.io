#include <gubg/ip/Socket.hpp>
#include <gubg/ip/my/descriptor.hpp>
#include <gubg/mss.hpp>
#include <cassert>

#include <gubg/platform.h>
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <fcntl.h>

namespace gubg { namespace ip { 

    Socket::Socket(Type type, Version version)
    {
        create(type, version);
    }
    Socket::Socket(Socket &&dying): descriptor_(dying.descriptor_), is_blocking_(dying.is_blocking_), type_(dying.type_), version_(dying.version_)
    {
        dying.descriptor_ = -1;
    }
    Socket::~Socket()
    {
        close();
    }
    Socket &Socket::operator=(Socket &&dying)
    {
        if (this != &dying)
        {
            close();
            descriptor_ = dying.descriptor_;
            is_blocking_ = dying.is_blocking_;
            type_ = dying.type_;
            version_ = dying.version_;
            dying.descriptor_ = -1;
        }
        return *this;
    }

    //Getters
    Type Socket::type() const
    {
        return type_;
    }
    Version Socket::version() const
    {
        return version_;
    }
    bool Socket::is_blocking() const
    {
        return is_blocking_;
    }

    //Setters
    Socket &Socket::set_blocking(bool b)
    {
        is_blocking_ = b;
        if (my::is_valid(descriptor_))
            my::set_blocking(descriptor_, is_blocking_);
        return *this;
    }

    bool Socket::valid() const
    {
        return my::is_valid(descriptor_);
    }

    bool Socket::create(Type type, Version version, int descr)
    {
        close();

        type_ = type;
        version_ = version;

        if (my::is_valid(descr))
            descriptor_ = descr;
        else
            descriptor_ = my::create_socket(type_, version_);

        set_blocking(is_blocking_);

        return valid();
    }
    void Socket::close()
    {
        if (my::is_valid(descriptor_))
        {
            my::close_socket(descriptor_);
            descriptor_ = -1;
        }
    }

    ReturnCode Socket::reuse_address(bool b)
    {
        return my::reuse_address(descriptor_, b);
    }

    ReturnCode Socket::bind(const Endpoint &ep)
    {
        return my::bind(descriptor_, ep.as_sockaddr());
    }

    ReturnCode Socket::listen()
    {
        return my::listen(descriptor_);
    }

    ReturnCode Socket::accept(Socket &peer_socket, Endpoint &peer_ep)
    {
        MSS_BEGIN(ReturnCode);

        int peer_descr = -1;
        Address peer_address;
        Port peer_port;
        switch (const auto rc = my::accept(descriptor_, peer_descr, peer_address, peer_port))
        {
            case ReturnCode::WouldBlock: return rc; break;
            default: MSS(rc); break;
        }

        peer_socket.create(type_, version_, peer_descr);
        peer_ep.setup(peer_address, peer_port);

        MSS_END();
    }

    ReturnCode Socket::connect(const Endpoint &ep)
    {
        return my::connect(descriptor_, ep.as_sockaddr());
    }

    ReturnCode Socket::is_connected()
    {
        return my::is_connected(descriptor_);
    }

    ReturnCode Socket::send(const void *buffer, std::size_t size, std::size_t &offset)
    {
        MSS_BEGIN(ReturnCode);

        if (offset < size)
        {
            unsigned int nr_sent;
            auto ptr = (const char *)buffer;
            switch (const auto rc = my::send(descriptor_, nr_sent, ptr+offset, size-offset))
            {
                case ReturnCode::OK:
                    offset += nr_sent;
                    break;
                default: MSS(rc); break;
            }
        }

        MSS_END();
    }

    ReturnCode Socket::recv(void *buffer, std::size_t size, std::size_t &offset)
    {
        MSS_BEGIN(ReturnCode);

        if (offset < size)
        {
            unsigned int nr_received;
            auto ptr = (char *)buffer;
            switch (const auto rc = my::recv(descriptor_, nr_received, ptr+offset, size-offset))
            {
                case ReturnCode::OK:
                    offset += nr_received;
                    break;
                case ReturnCode::WouldBlock: return rc; break;
                default: MSS(rc); break;
            }
        }

        MSS_END();
    }

    ReturnCode Socket::sendto(unsigned int &nr_sent, const void *buffer, unsigned int size, const Endpoint &ep)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
#if GUBG_PLATFORM_API_WIN32
        const auto status = ::sendto(descriptor_, (const char *)buffer, size, 0, &ep.as_sockaddr(), sizeof(sockaddr));
#else
        const auto status = ::sendto(descriptor_, buffer, size, 0, &ep.as_sockaddr(), sizeof(sockaddr));
#endif
        MSS(status != -1, return ReturnCode::CouldNotSend);
        nr_sent = status;
        MSS_END();
    }
    ReturnCode Socket::recvfrom(unsigned int &nr_received, void *buffer, unsigned int size, Endpoint &ep)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
#if GUBG_PLATFORM_API_WIN32
        int fromlen = sizeof(sockaddr);
        const auto status = ::recvfrom(descriptor_, (char *)buffer, size, 0, &ep.as_sockaddr(), &fromlen);
#else
        socklen_t fromlen = sizeof(sockaddr);
        const auto status = ::recvfrom(descriptor_, buffer, size, 0, &ep.as_sockaddr(), &fromlen);
#endif
        if (status == -1)
        {
            switch (errno)
            {
                /* case EAGAIN: */
                case EWOULDBLOCK:
                    nr_received = 0;
                    return ReturnCode::WouldBlock;
                    break;
                default:
                    MSS(ReturnCode::CouldNotReceive);
                    break;
            }
        }
        else
        {
            nr_received = status;
        }
        MSS_END();
    }

    void Socket::stream(std::ostream &os) const
    {
        os << "[Socket](desc:" << descriptor_ << ")";
    }

} } 
