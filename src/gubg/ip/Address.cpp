#include "gubg/ip/Address.hpp"
#include "gubg/Range.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace gubg { namespace ip { 

    namespace  { 
        short int to_family(Version version)
        {
            switch (version)
            {
                case Version::V4: return AF_INET;
                case Version::V6: return AF_INET6;
            }
            return -1;
        }
    } 

    void Address::stream(std::ostream &os) const
    {
        os << "[Address](";
        switch (version_)
        {
            case Version::V4:
                os << "v4:";
                for (auto i = 0u; i < v4_parts_.size(); ++i)
                {
                    if (i > 0)
                        os << '.';
                    os << (int)v4_parts_[i];
                }
                break;
            case Version::V6:
                os << "v6:";
                break;
        }
        os << ")";
    }

    void Address::from_uint32(std::uint32_t addr)
    {
        version_ = Version::V4;
        auto part = v4_parts_.begin();
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
    }

    const sockaddr *Address::as_sockaddr(Port port) const
    {
        static_assert(sizeof(buffer_) == sizeof(sockaddr), "Buffer is too small to fit sockaddr");
        std::fill(RANGE(buffer_), 0);
        auto sa = (sockaddr_in *)buffer_.data();
        sa->sin_family = to_family(version_);
        sa->sin_port = port;
        std::uint32_t addr = 0;
        auto part = v4_parts_.rbegin();
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        sa->sin_addr.s_addr = addr;
        return (const sockaddr *)sa;
    }

    bool Address::operator==(const Address &rhs) const
    {
        return version_ == rhs.version_ && v4_parts_ == rhs.v4_parts_;
    }

} } 
