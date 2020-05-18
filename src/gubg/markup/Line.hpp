#ifndef HEADER_gubg_markup_Line_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markup_Line_hpp_ALREADY_INCLUDED

#include <gubg/markup/Style.hpp>
#include <string>
#include <vector>

namespace gubg { namespace markup { 

    class Line
    {
    public:
        template <typename Ftor>
        void each(Ftor &&ftor) const
        {
            for (const auto &part: parts_)
                ftor(part.txt, part.style);
        }

        void add(const std::string &txt, Style style = Style{})
        {
            parts_.emplace_back(txt, style);
        }

    private:
        struct Part
        {
            std::string txt;
            Style style;

            Part() {}
            Part(const std::string &txt, const Style &style): txt(txt), style(style) {}
        };
        std::vector<Part> parts_;
    };

} } 

#endif
