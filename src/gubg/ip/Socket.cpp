#include "gubg/ip/Socket.hpp"
#include "gubg/mss.hpp"
#include <cassert>
#include <sys/types.h>
#include <sys/socket.h>
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

    ReturnCode Socket::bind(const Address &address, Port port)
    {
        MSS_BEGIN(ReturnCode);
        MSS(descriptor_ != -1, return ReturnCode::InvalidDescriptor);
        const auto status = ::bind(descriptor_, address.as_sockaddr(port), sizeof(sockaddr));
        MSS(status != -1, return ReturnCode::CouldNotBind);
        MSS_END();
    }

    void Socket::stream(std::ostream &os) const
    {
        os << "[Socket](desc:" << descriptor_ << ")";
    }

} } 
