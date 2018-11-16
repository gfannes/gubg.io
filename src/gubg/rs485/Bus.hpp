#ifndef HEADER_gubg_rs485_Bus_hpp_ALREADY_INCLUDED
#define HEADER_gubg_rs485_Bus_hpp_ALREADY_INCLUDED

#include "gubg/rs485/Types.hpp"
#include "gubg/rs485/Endpoint.hpp"
#include <list>
#include <deque>
#include <cstddef>

namespace gubg { namespace rs485 { 

    class Bus
    {
    public:
        Endpoint::Ptr create_endpoint()
        {
            Endpoint::Ptr ptr(new Endpoint(send_queue_));
            endpoints_.push_back(ptr);
            return ptr;
        }

        void process()
        {
            for (; !send_queue_.empty(); send_queue_.pop_front())
            {
                const auto sender = send_queue_.front().sender.lock();
                if (!sender)
                    //Sender already died
                    continue;

                const auto byte = send_queue_.front().byte;

                for (auto it = endpoints_.begin(); it != endpoints_.end();)
                {
                    auto ptr = it->lock();
                    if (!ptr)
                    {
                        it = endpoints_.erase(it);
                        continue;
                    }
                    ++it;

                    if (sender == ptr)
                        //We don't send to the sender
                        continue;

                    ptr->receive_queue_.push_back(byte);
                }
            }
        }

    private:
        SendQueue send_queue_;
        std::list<Endpoint_wptr> endpoints_;
    };

} } 

#endif
