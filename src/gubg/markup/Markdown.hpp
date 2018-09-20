#ifndef HEADER_gubg_markup_Markdown_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markup_Markdown_hpp_ALREADY_INCLUDED

#include "gubg/markup/Data.hpp"
#include "gubg/mss.hpp"

namespace gubg { namespace markup { 

    class Markdown
    {
    public:
        struct Info
        {
            unsigned int level = 0;
        };

        template <typename Data>
        bool open(Info &info, const Data &) const
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        bool open(std::ostream &os, const Info &info, const Document &doc) const
        {
            MSS_BEGIN(bool);
            os << doc.title << std::endl;
            os << std::string(doc.title.size(), '=') << std::endl;
            os << std::endl;
            MSS_END();
        }
        bool open(std::ostream &os, const Info &info, const Section &sec) const
        {
            MSS_BEGIN(bool);
            os << "## " << sec.title << std::endl;
            os << std::endl;
            MSS_END();
        }

        template <typename Data>
        bool close(Info &info, const Data &) const
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        template <typename Data>
        bool close(std::ostream &os, const Info &info, const Data &) const
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
    private:
    };

} } 

#endif
