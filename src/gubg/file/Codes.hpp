#ifndef HEADER_gubg_file_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace file {

    enum class ReturnCode
    {
        OK,
        Error,

        ExpectedRegular,
        ExpectedDirectory,
        CouldNotOpenFile,
        CouldNotOpenDir,
        CouldNotReadEntry,
        CouldNotWriteFile,
        CouldNotGetCWD,
        Skip,
        Stop,
        FileDoesNotExist,
        UnknownFileType,
        WrongExtension,
        TreeAlreadyAdded,
        CouldNotResolve,
        CouldNotRemove,
        FailedToAccept,
        FailedToOpen,
        UnknownType,
        FailedToSelect,
        NotSupported,
        AlreadyAcceptedFile,
        InvalidMaxDesc,
        PeerClosedConnection,
        FailedToWrite,
        FailedToRead,
    };

} }

#endif
