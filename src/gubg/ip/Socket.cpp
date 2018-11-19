#include "gubg/ip/Socket.hpp"
#include "gubg/mss.hpp"
#include <cassert>

#include "gubg/platform.h"
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <fcntl.h>

namespace gubg { namespace ip { 

    namespace  { 
        int to_domain(Version version)
        {
            switch (version)
            {
                case Version::V4: return PF_INET;
                case Version::V6: return PF_INET6;
            }
            assert(false);
            return -1;
        }
        int to_type(Type type)
        {
            switch (type)
            {
                case Type::UDP: return SOCK_DGRAM;
                case Type::TCP: return SOCK_STREAM;
            }
            assert(false);
            return -1;
        }
    } 

    Socket::Socket(Type type, Version version)
    {
        setup(type, version);
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

    bool Socket::setup(Type type, Version version)
    {
        close();

        type_ = type;
        version_ = version;
        is_blocking_ = true;
        descriptor_ = ::socket(to_domain(version_), to_type(type_), 0);

        return valid();
    }

    bool Socket::valid() const
    {
        return descriptor_ != -1;
    }

    void Socket::close()
    {
        if (descriptor_ == -1)
            return;
#if GUBG_PLATFORM_API_WIN32
        ::closesocket(descriptor_);
#else
        ::close(descriptor_);
#endif
        descriptor_ = -1;
        is_blocking_ = true;
    }

    ReturnCode Socket::get_blocking(bool &block) const
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        block = is_blocking_;
        MSS_END();
    }
    ReturnCode Socket::set_blocking(bool block)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
#if GUBG_PLATFORM_API_WIN32
        u_long mode = !block;
        ::ioctlsocket(descriptor_, FIONBIO, &mode);
#else
        auto flags = ::fcntl(descriptor_, F_GETFL, 0);
        if (block)
            flags &= ~O_NONBLOCK;
        else
            flags |= O_NONBLOCK;
        ::fcntl(descriptor_, F_SETFL, flags);
#endif
        is_blocking_ = block;
        MSS_END();
    }

    ReturnCode Socket::bind(const Endpoint &ep)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        const auto status = ::bind(descriptor_, &ep.as_sockaddr(), sizeof(sockaddr));
        MSS(status != -1, return ReturnCode::CouldNotBind);
        MSS_END();
    }

    ReturnCode Socket::sendto(unsigned int &sent, const void *buffer, unsigned int size, const Endpoint &ep)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
#if GUBG_PLATFORM_API_WIN32
        const auto status = ::sendto(descriptor_, (const char *)buffer, size, 0, &ep.as_sockaddr(), sizeof(sockaddr));
#else
        const auto status = ::sendto(descriptor_, buffer, size, 0, &ep.as_sockaddr(), sizeof(sockaddr));
#endif
        MSS(status != -1, return ReturnCode::CouldNotSend);
        sent = status;
        MSS_END();
    }

    ReturnCode Socket::recvfrom(unsigned int &recv, void *buffer, unsigned int size, Endpoint &ep)
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
                    recv = 0;
                    return ReturnCode::WouldBlock;
                    break;
                default:
                    MSS(ReturnCode::CouldNotReceive);
                    break;
            }
        }
        else
        {
            recv = status;
        }
        MSS_END();
    }

    void Socket::stream(std::ostream &os) const
    {
        os << "[Socket](desc:" << descriptor_ << ")";
    }

} } 
