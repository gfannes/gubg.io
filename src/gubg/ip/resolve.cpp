#include "gubg/ip/resolve.hpp"
#include "gubg/mss.hpp"
#include <cstring>

#include "gubg/platform/os_api.h"
#if GUBG_API_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace gubg { namespace ip { 

    ReturnCode resolve(Address &address, const std::string &hostname)
    {
        MSS_BEGIN(ReturnCode, "");

        struct addrinfo hints;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;//IPv4 for now
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

        struct addrinfo *info = nullptr;
        const int status = ::getaddrinfo(hostname.c_str(), nullptr, &hints, &info);
        MSS(status == 0, return ReturnCode::UnkownHost);
        MSS(!!info, return ReturnCode::UnkownHost);

        const auto &addr = *(struct sockaddr_in *)info->ai_addr;
        address.from_uint32(addr.sin_addr.s_addr);

        ::freeaddrinfo(info);

        MSS_END();
    }

} } 
