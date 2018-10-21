#include "gubg/ip/Socket.hpp"
#include "gubg/mss.hpp"
#include <cassert>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

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
    Socket::Socket(Socket &&dying): descriptor_(dying.descriptor_), type_(dying.type_), version_(dying.version_)
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
        ::close(descriptor_);
        descriptor_ = -1;
    }

    ReturnCode Socket::get_blocking(bool &block) const
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        const auto flags = ::fcntl(descriptor_, F_GETFL, 0);
        std::cout << std::hex << flags << std::endl;
        block = !(flags & O_NONBLOCK);
        MSS_END();
    }
    ReturnCode Socket::set_blocking(bool block)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        auto flags = ::fcntl(descriptor_, F_GETFL, 0);
        std::cout << std::hex << flags << std::endl;
        std::cout << std::hex << (~O_NONBLOCK) << std::endl;
        if (block)
            flags &= ~O_NONBLOCK;
        else
            flags |= O_NONBLOCK;
        std::cout << std::hex << flags << std::endl;
        ::fcntl(descriptor_, F_SETFL, flags);
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
        const auto status = ::sendto(descriptor_, buffer, size, 0, &ep.as_sockaddr(), sizeof(sockaddr));
        MSS(status != -1, return ReturnCode::CouldNotSend);
        sent = status;
        MSS_END();
    }

    ReturnCode Socket::recvfrom(unsigned int &recv, void *buffer, unsigned int size, Endpoint &ep)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        socklen_t fromlen = sizeof(sockaddr);
        const auto status = ::recvfrom(descriptor_, buffer, size, 0, &ep.as_sockaddr(), &fromlen);
        if (status == -1)
        {
            switch (errno)
            {
                /* case EAGAIN: */
                case EWOULDBLOCK:
                    recv = 0;
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
