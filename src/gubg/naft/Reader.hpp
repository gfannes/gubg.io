#ifndef HEADER_gubg_naft_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Reader_hpp_ALREADY_INCLUDED

#include "gubg/naft/Range.hpp"
#include "gubg/mss.hpp"
#include <list>

namespace gubg { namespace naft { 

    class Reader
    {
    public:
        Reader(const std::string &str): range_(str)
        {
            path_.emplace_back();
            top_().block = range_;
        }

        bool pop_name(const std::string &name)
        {
            MSS_BEGIN(bool);
            Info info;
            MSS(pop_info_(info));
            MSS_END();
        }
        bool pop_type(const std::string &type)
        {
            MSS_BEGIN(bool);
            Info info;
            MSS(pop_info_(info));
            MSS_END();
        }

    private:
        struct Info
        {
            Strange tag;
            Attr attr;
            Range block;
        };

        bool pop_info_(Info &info)
        {
            MSS_BEGIN(bool);
            auto &block = top_().block;
            MSS_Q(block.pop_tag(info.tag));
            block.pop_attr(info.attr);
            block.pop_block(info.block);
            MSS_END();
        }

        Range range_;

        Info &top_() {return path_.back();}
        std::list<Info> path_;
    };

} } 

#endif
