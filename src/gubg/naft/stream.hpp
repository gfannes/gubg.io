#ifndef HEADER_gubg_naft_stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_stream_hpp_ALREADY_INCLUDED

#include <gubg/naft/Document.hpp>

#include <ostream>

namespace gubg { namespace naft {
    template<typename T>
    concept ImplementsWrite = requires {
        {
            std::declval<const T>().write(std::declval<Node &>())
        } -> std::same_as<void>;
    };
}} // namespace gubg::naft

namespace std {

template<gubg::naft::ImplementsWrite T>
std::ostream &operator<<(std::ostream &os, const T &v)
{
    gubg::naft::Document doc{os};
    v.write(doc);
    return os;
}

} // namespace std

#endif
