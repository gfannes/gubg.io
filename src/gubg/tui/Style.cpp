#include <gubg/tui/Style.hpp>

#include <oof.h>

namespace gubg { namespace tui {

    namespace my {
        oof::color to_color(Style::Color color)
        {
            oof::color res;
            switch (color)
            {
                case Style::White: res = {255, 255, 255}; break;
                case Style::Red: res = {255, 0, 0}; break;
                case Style::Green: res = {63, 220, 63}; break;
                case Style::Blue: res = {63, 63, 220}; break;
                case Style::Orange: res = {255, 127, 0}; break;
            }
            return res;
        }
    } // namespace my

    Style Style::attention(unsigned int level)
    {
        Style style;
        switch (level)
        {
            case 0: style.color = Style::White; break;
            case 1: style.color = Style::Blue; break;
            case 2: style.color = Style::Orange; break;
            case 3: style.color = Style::Green; break;
            case 4: style.color = Style::Red; break;
            default: style.color = Style::Red; break;
        }
        return style;
    }

    std::ostream &operator<<(std::ostream &os, const Style &style)
    {
        os << oof::fg_color(my::to_color(style.color)) << oof::bold(style.bold);
        return os;
    }

}} // namespace gubg::tui
