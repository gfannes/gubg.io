#ifndef HEADER_gubg_ip_Port_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Port_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <ostream>

namespace gubg { namespace ip { 

    class Port
    {
    public:
        bool valid() const {return hbo_ != 0;}

        //Host byte order
        void from_hbo(std::uint16_t addr_hbo);
        std::uint16_t as_hbo() const;

        //Network byte order
        void from_nbo(std::uint16_t addr_nbo);
        std::uint16_t as_nbo() const;

        bool operator==(const Port &rhs) const;
        bool operator!=(const Port &rhs) const {return !operator==(rhs);}

        void stream(std::ostream &os) const;

    private:
        std::uint16_t hbo_ = 0;
    };

    inline std::ostream &operator<<(std::ostream &os, const Port &port)
    {
        port.stream(os);
        return os;
    }

} } 

#endif
