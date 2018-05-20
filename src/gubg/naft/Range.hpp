#ifndef HEADER_gubg_naft_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Range_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
#include <map>

namespace gubg { namespace naft { 

    using Attr = std::map<std::string, std::string>;

    class Range
    {
    public:
        Range(){}
        Range(const std::string &str): strange_(str) {strip_();}

        bool empty() const {return strange_.empty();}
        std::string str() const {return strange_.str();}

        bool pop_tag(std::string &tag)
        {
            if (!strange_.pop_bracket(tag, "[]"))
                return false;
            strip_();
            return true;
        }
        bool pop_tag(const std::string &tag)
        {
            auto sp = strange_;
            std::string tmp;
            if (!strange_.pop_bracket(tmp, "[]"))
            {
                strange_ = sp;
                return false;
            }
            if (tmp != tag)
            {
                strange_ = sp;
                return false;
            }
            strip_();
            return true;
        }

        void pop_attr(Attr &attr)
        {
            Strange kv;
            std::string k,v;
            while (strange_.pop_bracket(kv, "()"))
            {
                strip_();
                v.clear();
                if (kv.pop_until(k, ':'))
                    kv.pop_all(v);
                else
                    kv.pop_all(k);
                attr.emplace(k, v);
            }
        }
        bool pop_block(Range &block)
        {
            if (!strange_.pop_bracket(block.strange_, "{}"))
                return false;
            block.strip_();
            strip_();
            return true;
        }
    private:
        const std::string whitespace_ = " \t\n\r";
        void strip_() {strange_.strip(whitespace_);}

        Strange strange_;
    };
} } 

#endif
