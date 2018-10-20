#include "gubg/ip/Address.hpp"

namespace gubg { namespace ip { 

    void Address::stream(std::ostream &os) const
    {
        os << "[Address](";
        switch (type_)
        {
            case V4:
                os << "v4:";
                for (auto i = 0u; i < v4_parts_.size(); ++i)
                {
                    if (i > 0)
                        os << '.';
                    os << (int)v4_parts_[i];
                }
                break;
            case V6:
                os << "v6:";
                break;
        }
        os << ")";
    }

    void Address::from_uint32(std::uint32_t addr)
    {
        type_ = V4;
        auto part = v4_parts_.begin();
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
        *part++ = (addr & 0xff); addr >>= 8;
    }

    bool Address::operator==(const Address &rhs) const
    {
        return type_ == rhs.type_ && v4_parts_ == rhs.v4_parts_;
    }

} } 
