#include <gubg/ip/my/descriptor.hpp>
#include <gubg/mss.hpp>
#include <cassert>

#include <gubg/platform.h>
#if GUBG_PLATFORM_API_WIN32
#include <winsock2.h>
#include <stdint.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <fcntl.h>

#if GUBG_PLATFORM_API_WIN32
typedef int socklen_t;
typedef char * buffer_type;
typedef const char * const_buffer_type;
#else
typedef void * buffer_type;
typedef const void * const_buffer_type;
#endif

namespace gubg { namespace ip { namespace my { 

    namespace  { 
        int to_domain(Version version)
        {
            switch (version)
            {
                case Version::V4: return PF_INET;
                case Version::V6: return PF_INET6;
            }
            assert(false);
            return -1;
        }
        int to_type(Type type)
        {
            switch (type)
            {
                case Type::UDP: return SOCK_DGRAM;
                case Type::TCP: return SOCK_STREAM;
            }
            assert(false);
            return -1;
        }

        bool last_error_is_would_block()
        {
#if GUBG_PLATFORM_API_WIN32
            const auto my_error = WSAGetLastError();
#define unroll_wouldblock_errors(x) x(WSAEWOULDBLOCK)
#else
            const auto my_error = errno;
#define unroll_wouldblock_errors(x) x(EAGAIN) x(EWOULDBLOCK) x(EINPROGRESS) x(ENOTCONN)
#endif

#define return_true_if_same(value) if (my_error == value) return true;
            unroll_wouldblock_errors(return_true_if_same)
#undef unroll_wouldblock_errors

            return false;
        }
    } 

    template <typename T>
    ReturnCode setsockopt(int descr, int level, int optname, const T &v)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::setsockopt(descr, level, optname, (const_buffer_type)&v, sizeof(v));
        
        MSS(status != -1, return ReturnCode::CouldNotSetSockOpt);

        MSS_END();
    }

    int create_socket(Type type, Version version)
    {
        const int descr = ::socket(to_domain(version), to_type(type), 0);
        return descr;
    }

    bool close_socket(int descr)
    {
        MSS_BEGIN(bool);

        MSS(is_valid(descr));

#if GUBG_PLATFORM_API_WIN32
        ::closesocket(descr);
#else
        ::close(descr);
#endif

        MSS_END();
    }

    bool is_valid(int descr)
    {
        return descr != -1;
    }

    ReturnCode set_blocking(int descr, bool b)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

#if GUBG_PLATFORM_API_WIN32
        u_long mode = !b;
        ::ioctlsocket(descr, FIONBIO, &mode);
#else
        auto flags = ::fcntl(descr, F_GETFL, 0);
        if (b)
            flags &= ~O_NONBLOCK;
        else
            flags |= O_NONBLOCK;
        ::fcntl(descr, F_SETFL, flags);
#endif

        MSS_END();
    }

    ReturnCode bind(int descr, const struct sockaddr &sa)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::bind(descr, &sa, sizeof(sa));
        MSS(status != -1, return ReturnCode::CouldNotBind);

        MSS_END();
    }

    ReturnCode listen(int descr)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::listen(descr, 20);
        MSS(status != -1, return ReturnCode::CouldNotListen);

        MSS_END();
    }

    ReturnCode reuse_address(int descr, bool b)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

#if GUBG_PLATFORM_API_WIN32
        const int optname = SO_EXCLUSIVEADDRUSE;
#else
        const int optname = SO_REUSEADDR;
#endif
        const int b_as_int = b;
        switch (const auto rc = my::setsockopt(descr, SOL_SOCKET, optname, b_as_int))
        {
            case ReturnCode::CouldNotSetSockOpt:
                MSS(ReturnCode::CouldNotReuseAddress);
                break;
            default: MSS(rc); break;
        }

        MSS_END();
    }

    ReturnCode accept(int descr, int &peer_descr, Address &peer_address, Port &peer_port)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        struct sockaddr_storage peer_sa;
        socklen_t sa_size = sizeof(peer_sa);
        const auto status = ::accept(descr, (struct sockaddr *)&peer_sa, &sa_size);
        if (status == -1)
        {
            if (last_error_is_would_block())
                return ReturnCode::WouldBlock;
            MSS(ReturnCode::CouldNotAccept);
        }

        peer_descr = status;

        {
            const auto si_ptr = (const sockaddr_in *)&peer_sa;
            peer_address.from_nbo(si_ptr->sin_addr.s_addr);
            peer_port.from_nbo(si_ptr->sin_port);
        }

        MSS_END();
    }

    ReturnCode connect(int descr, const struct sockaddr &sa)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::connect(descr, &sa, sizeof(struct sockaddr));
        if (status == -1)
        {
            if (last_error_is_would_block())
                return ReturnCode::WouldBlock;
            MSS(ReturnCode::CouldNotConnect);
        }

        MSS_END();
    }

    ReturnCode is_connected(int descr)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        struct sockaddr sa{};
        socklen_t sa_size = sizeof(sa);

        //When connecting using a nonblocking socket, we use ::getpeername() to check if the connection is already established
        const auto status = ::getpeername(descr, &sa, &sa_size);
        if (status == -1)
        {
            if (last_error_is_would_block())
                return ReturnCode::WouldBlock;
            MSS(ReturnCode::CouldNotCheckConnection);
        }

        MSS_END();
    }

    ReturnCode send(int descr, unsigned int &nr_sent, const void *buffer, unsigned int size)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::send(descr, (const_buffer_type)buffer, size, 0);
        if (status == -1)
        {
            if (last_error_is_would_block())
                return ReturnCode::WouldBlock;
            MSS(ReturnCode::CouldNotSend);
        }
        nr_sent = status;

        MSS_END();
    }

    ReturnCode recv(int descr, unsigned int &nr_received, void *buffer, unsigned int size)
    {
        MSS_BEGIN(ReturnCode);

        MSS(is_valid(descr), return ReturnCode::InvalidDescriptor);

        const auto status = ::recv(descr, (buffer_type)buffer, size, 0);
        if (status == -1)
        {
            if (last_error_is_would_block())
                return ReturnCode::WouldBlock;
            MSS(ReturnCode::CouldNotReceive);
        }
        nr_received = status;

        MSS_END();
    }

} } } 
