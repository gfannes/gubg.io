#ifndef HEADER_gubg_ip_tcp_Client_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_tcp_Client_hpp_ALREADY_INCLUDED

#include <gubg/ip/Codes.hpp>
#include <gubg/ip/Endpoint.hpp>
#include <gubg/ip/tcp/Connection.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace ip { namespace tcp { 

    class Client
    {
    public:
        Client(const Endpoint &server_ep): server_ep_(server_ep) {}

        ReturnCode process(Connection &connection)
        {
            MSS_BEGIN(ReturnCode);
            switch (connection.state_)
            {
                case Connection::State::Unconnected:
                    connection.socket.create(Type::TCP, Version::V4);
                    MSS(server_ep_.valid());
                    switch (const auto rc = connection.socket.connect(server_ep_))
                    {
                        case ReturnCode::OK:
                            MSS(connection.change_state_(Connection::State::Connected));
                            break;
                        case ReturnCode::WouldBlock:
                            MSS(connection.change_state_(Connection::State::Connecting));
                            break;
                        default:
                            MSS(rc);
                            break;
                    }
                    break;
                case Connection::State::Connecting:
                    switch (const auto rc = connection.socket.is_connected())
                    {
                        case ReturnCode::WouldBlock: return ReturnCode::OK; break;
                        default: MSS(rc); break;
                    }
                    connection.peer_ep_ = server_ep_;
                    MSS(connection.change_state_(Connection::State::Connected));
                    break;
                case Connection::State::Connected:
                    break;
            }
            MSS_END();
        }

    private:
        const Endpoint server_ep_;
    };

} } } 

#endif
