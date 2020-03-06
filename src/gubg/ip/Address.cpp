#include "gubg/ip/Address.hpp"
#include "gubg/Range.hpp"

#include "gubg/platform.h"
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace gubg { namespace ip { 

    bool Address::valid() const
    {
        for (auto part: v4_parts_)
            if (part != 0)
                return true;
        return false;
    }

    Address Address::localhost()
    {
        return Address{127,0,0,1};
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

    void Address::from_nbo(std::uint32_t addr_nbo)
    {
        version_ = Version::V4;
        auto part = v4_parts_.begin();
        *part++ = (addr_nbo & 0xff); addr_nbo >>= 8;
        *part++ = (addr_nbo & 0xff); addr_nbo >>= 8;
        *part++ = (addr_nbo & 0xff); addr_nbo >>= 8;
        *part++ = (addr_nbo & 0xff); addr_nbo >>= 8;
    }

    std::uint32_t Address::as_nbo() const
    {
        std::uint32_t addr = 0;
        //Note: reverse iteration
        auto part = v4_parts_.rbegin();
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        addr <<= 8; addr |= *part++;
        return addr;
    }

    bool Address::operator==(const Address &rhs) const
    {
        return version_ == rhs.version_ && v4_parts_ == rhs.v4_parts_;
    }

} } 
