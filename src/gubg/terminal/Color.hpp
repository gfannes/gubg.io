#ifndef HEADER_gubg_terminal_Color_hpp_ALREADY_INCLUDED
#define HEADER_gubg_terminal_Color_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace terminal { 

    class Color
    {
    public:
        Color(unsigned int v): v_(v) {}

        void stream(std::ostream &os) const
        {
            os << "\u001b[38;5;" << v_ << "m";
        }

    private:
        unsigned int v_ = 0;
    };

    std::ostream &operator<<(std::ostream &os, const Color &color)
    {
        color.stream(os);
        return os;
    }

} } 

#endif
