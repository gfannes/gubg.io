#ifndef HEADER_gubg_rs485_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_rs485_Types_hpp_ALREADY_INCLUDED

#include <cstddef>
#include <memory>
#include <deque>

namespace gubg { namespace rs485 { 

    class Bus;
    class Endpoint;
    using Endpoint_wptr = std::weak_ptr<Endpoint>;

    using ReceiveQueue = std::deque<std::byte>;

    struct OutPOD
    {
        Endpoint_wptr sender;
        std::byte byte;
        OutPOD(const Endpoint_wptr &sender, std::byte byte): sender(sender), byte(byte) {}
    };
    using SendQueue = std::deque<OutPOD>;

} } 

#endif
