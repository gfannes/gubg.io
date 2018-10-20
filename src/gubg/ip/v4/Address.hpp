#ifndef HEADER_gubg_ip_v4_Address_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_v4_Address_hpp_ALREADY_INCLUDED

#include <array>
#include <cstdint>
#include <ostream>

namespace gubg { namespace ip { namespace v4 { 

    class Address
    {
    public:
        Address(){}
        Address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d): parts_({a, b, c, d}) {}

        void stream(std::ostream &) const;
        void from_uint(std::uint32_t addr);

        bool operator==(const Address &rhs) const {return parts_ == rhs.parts_;}
        bool operator!=(const Address &rhs) const {return !operator==(rhs);}

    private:
        std::array<std::uint8_t, 4> parts_{};
    };

    std::ostream &operator<<(std::ostream &, const Address &);

} } } 

#endif
