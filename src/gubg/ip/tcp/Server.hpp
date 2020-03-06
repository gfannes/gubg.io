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
                case State::Idle:
                    MSS(change_state_(State::Setup));
                    break;
                case State::Accepting:
                    switch (const auto rc = acceptor_.accept(connection.socket, connection.peer_ep_))
                    {
                        case ReturnCode::WouldBlock: return ReturnCode::OK; break;
                        default: MSS(rc); break;
                    }
                    MSS(connection.change_state_(Connection::State::Connected));
                    break;
                case State::Error:
                    MSS(change_state_(State::Idle));
                    break;
            }
            MSS_END();
        }

    private:
        enum class State {Idle, Setup, Accepting, Error, };

        ReturnCode error_(const std::string &msg)
        {
            MSS_BEGIN(ReturnCode);
            std::cout << "Error: " << msg << std::endl;
            MSS(change_state_(State::Error));
            MSS_END();
        }

        ReturnCode change_state_(State new_state)
        {
            MSS_BEGIN(ReturnCode);

            if (new_state == state_)
                return ReturnCode::OK;

            state_ = new_state;

            switch (state_)
            {
                case State::Setup:
                    acceptor_.set_blocking(false);
                    acceptor_.create(Type::TCP, Version::V4);
                    MSS(acceptor_.reuse_address(true), return error_("Could not reuse address"));
                    MSS(acceptor_.bind(listen_ep_), return error_("Could not bind"));
                    MSS(acceptor_.listen(), return error_("Could not listen"));
                    return change_state_(State::Accepting);
                    break;
                default:
                    break;
            }

            MSS_END();
        }

        const Endpoint listen_ep_;
        Socket acceptor_;
        State state_ = State::Idle;
    };

} } } 

#endif
