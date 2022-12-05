#ifndef HEADER_gubg_tui_Style_hpp_ALREAD_INCLUDED
#define HEADER_gubg_tui_Style_hpp_ALREAD_INCLUDED

#include <ostream>

namespace gubg { namespace tui {

    class Style
    {
    public:
        enum Color
        {
            White,
            Blue,
            Green,
            Orange,
            Red,
        };

        Color color = White;
        bool bold = false;

        static Style attention(unsigned int level);

    private:
    };

    std::ostream &operator<<(std::ostream &, const Style &);

}} // namespace gubg::tui

#endif
