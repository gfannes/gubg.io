#include "gubg/ip/v4/Address.hpp"

namespace gubg { namespace ip { namespace v4 { 

    void Address::stream(std::ostream &os) const
    {
        os << "[Address](v4:";
        for (auto i = 0u; i < parts_.size(); ++i)
        {
            if (i > 0)
                os << '.';
            os << (int)parts_[i];
        }
        os << ")";
    }

    void Address::from_uint(std::uint32_t addr)
    {
        auto part = parts_.begin();
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
    }

    std::ostream &operator<<(std::ostream &os, const Address &address)
    {
        address.stream(os);
        return os;
    }

} } } 
