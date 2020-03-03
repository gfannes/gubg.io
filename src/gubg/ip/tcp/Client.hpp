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
            MSS_END();
        }

    private:
        const Endpoint server_ep_;
    };

} } } 

#endif
