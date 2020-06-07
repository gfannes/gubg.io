#ifndef HEADER_gubg_markup_Document_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markup_Document_hpp_ALREADY_INCLUDED

#include <gubg/markup/Line.hpp>
#include <vector>

namespace gubg { namespace markup { 

    class Document
    {
    public:
        void clear() {*this = Document{};}

        template <typename Ftor>
        void each(Ftor &&ftor) const
        {
            std::size_t line_ix = 0;

            auto my_ftor = [&](const std::string &txt, const Style &style)
            {
                ftor(line_ix, txt, style);
            };

            for (const auto &line: lines_)
            {
                line.each(my_ftor);
                ++line_ix;
            }
        }

        template <typename Ftor>
        void add_line(Ftor &&ftor)
        {
            lines_.emplace_back();
            ftor(lines_.back());
        }

    private:
        std::vector<Line> lines_;
    };

} } 

#endif
