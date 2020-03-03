#ifndef HEADER_gubg_ip_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_Codes_hpp_ALREADY_INCLUDED

namespace gubg { namespace ip { 

    enum class ReturnCode
    {
        OK,
        Error,

        InvalidDescriptor,
        UnkownHost,

        CouldNotSetSockOpt,

        CouldNotBind,
        CouldNotListen,
        CouldNotReuseAddress,
        CouldNotAccept,
        CouldNotConnect,
        CouldNotCheckConnection,
        CouldNotSend,
        CouldNotReceive,

        WouldBlock,
    };

} } 

#endif
