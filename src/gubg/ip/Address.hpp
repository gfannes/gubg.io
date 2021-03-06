#ifndef HEADER_gubg_ip_Address_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Address_hpp_ALREADY_INCLUDED

#include "gubg/ip/Types.hpp"
#include "gubg/std/optional.hpp"
#include <array>
#include <cstdint>
#include <ostream>

extern "C"
{
    struct sockaddr;
};

namespace gubg { namespace ip { 

    class Address
    {
    public:
        Address(){}
        Address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d): v4_parts_({a, b, c, d}) {}
        static Address localhost();

        bool valid() const;

        Version version() const {return version_;}

        void stream(std::ostream &) const;

        void from_nbo(std::uint32_t addr_nbo);
        std::uint32_t as_nbo() const;

        bool operator==(const Address &rhs) const;
        bool operator!=(const Address &rhs) const {return !operator==(rhs);}

    private:
        Version version_ = Version::V4;
        std::array<std::uint8_t, 4> v4_parts_{};
    };

    inline std::ostream &operator<<(std::ostream &os, const Address &address)
    {
        address.stream(os);
        return os;
    }

} } 

#endif
