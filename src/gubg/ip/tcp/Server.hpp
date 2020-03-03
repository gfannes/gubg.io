#ifndef HEADER_gubg_ip_tcp_Server_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_tcp_Server_hpp_ALREADY_INCLUDED

#include <gubg/ip/Codes.hpp>
#include <gubg/ip/tcp/Connection.hpp>
#include <gubg/ip/Endpoint.hpp>
#include <gubg/ip/Socket.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace ip { namespace tcp { 

    class Server
    {
    public:
        Server(const Endpoint &listen_ep): listen_ep_(listen_ep) {}

        ReturnCode process(Connection &connection)
        {
            MSS_BEGIN(ReturnCode);
            switch (state_)
            {
            }
            MSS_END();
        }

    private:
        enum class State {Idle, };

        const Endpoint listen_ep_;
        Socket acceptor_;
        State state_ = State::Idle;
    };

} } } 

#endif
