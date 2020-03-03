#ifndef HEADER_gubg_ip_my_descriptor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_my_descriptor_hpp_ALREADY_INCLUDED

#include <gubg/ip/Types.hpp>
#include <gubg/ip/Codes.hpp>
#include <gubg/ip/Address.hpp>
#include <gubg/ip/Port.hpp>

#include <gubg/platform.h>
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace gubg { namespace ip { namespace my { 

    int create_socket(Type, Version);
    bool close_socket(int descr);
    bool is_valid(int descr);

    ReturnCode set_blocking(int descr, bool);
    ReturnCode bind(int descr, const struct sockaddr &);
    ReturnCode listen(int descr);
    ReturnCode reuse_address(int descr, bool);
    ReturnCode accept(int descr, int &peer_descr, Address &peer_address, Port &peer_port);
    ReturnCode connect(int descr, const struct sockaddr &);
    ReturnCode is_connected(int descr);
    ReturnCode send(int descr, unsigned int &nr_sent, const void *buffer, unsigned int size);
    ReturnCode recv(int descr, unsigned int &nr_received, void *buffer, unsigned int size);

} } } 

#endif
