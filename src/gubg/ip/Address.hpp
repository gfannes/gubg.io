#ifndef HEADER_gubg_ip_Address_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Address_hpp_ALREADY_INCLUDED

#include <optional>
#include <array>
#include <cstdint>
#include <ostream>

namespace gubg { namespace ip { 

    class Address
    {
    public:
        enum Type { V4, V6, };

        Address(){}
        Address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d): v4_parts_({a, b, c, d}) {}

        void stream(std::ostream &) const;
        void from_uint32(std::uint32_t addr);

        bool operator==(const Address &rhs) const;
        bool operator!=(const Address &rhs) const {return !operator==(rhs);}

    private:
        Type type_ = V4;
        std::array<std::uint8_t, 4> v4_parts_{};
    };

    inline std::ostream &operator<<(std::ostream &os, const Address &address)
    {
        address.stream(os);
        return os;
    }

} } 

#endif
