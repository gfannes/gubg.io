#ifndef HEADER_gubg_rs485_Endpoint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_rs485_Endpoint_hpp_ALREADY_INCLUDED

#include "gubg/rs485/Types.hpp"
#include <memory>
#include <cstddef>
#include <deque>

namespace gubg { namespace rs485 { 

    class Endpoint: public std::enable_shared_from_this<Endpoint>
    {
    public:
        using Ptr = std::shared_ptr<Endpoint>;
        using WPtr = std::weak_ptr<Endpoint>;

        bool receive(std::byte &byte)
        {
            if (receive_queue_.empty())
                return false;
            byte = receive_queue_.front();
            receive_queue_.pop_front();
            return true;
        }
        void send(std::byte byte)
        {
            send_queue_.emplace_back(weak_from_this(), byte);
        }

    private:
        friend class Bus;
        Endpoint(SendQueue &send_queue): send_queue_(send_queue) {} 

        SendQueue &send_queue_;
        ReceiveQueue receive_queue_;
    };

} } 

#endif
