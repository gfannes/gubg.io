#ifndef HEADER_gubg_parse_xml_Codes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_xml_Codes_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"

namespace gubg { namespace xml {

    enum class ReturnCode
    {
        OK, Error,

        XMLEmpty, CloseTagMismatch, DuplicateAttributeName, NonPrintable, UnknownEscape,
        UnexpectedRawChar, NoVersionFound,
    };

} }

#endif
