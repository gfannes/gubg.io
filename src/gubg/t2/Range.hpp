#ifndef HEADER_gubg_t2_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Range_hpp_ALREADY_INCLUDED

#include "gubg/t2/Span.hpp"
#include "gubg/mss.hpp"

namespace gubg { namespace t2 { 

    class Range
    {
    public:
        Range() {}
        Range(const Byte *begin, const Byte *end): span_(begin, end) {}
        Range(const Range &rhs): span_(rhs.span_) {}

        bool empty() const {return span_.empty();}
        size_t size() const {return span_.size();}

        bool pop_tag(Data &tag)
        {
            return pop_value_(tag, md_open_tag);
        }
        bool pop_tag_if(Data wanted_tag)
        {
            auto sp = span_;
            Data tag;
            if (!pop_value_(tag, md_open_tag) || tag != wanted_tag)
            {
                span_ = sp;
                return false;
            }
            return true;
        }

        bool pop_attr(Data &key, Data &value)
        {
            auto sp = span_;
            if (!pop_value_(key, md_open_attr) || !pop_value_(value, md_open_attr))
            {
                span_ = sp;
                return false;
            }
            return true;
        }

        bool pop_block(Range &block)
        {
            const Byte *it = span_.begin();
            for (unsigned int level = 0; it != span_.end(); ++it)
            {
                if ((*it & mask_md) == md_open_tag)
                    ++level;
                else if (*it == md_close_block)
                {
                    if (level == 0)
                        break;
                    --level;
                }
            }
            block = Range(span_.begin(), it);
            span_.assign(it, span_.end());
            return !block.empty();
        }

    private:
        bool pop_value_(Data &value, const Byte md)
        {
            MSS_BEGIN(bool);
            Byte b;
            MSS_Q(span_.pop(b));
            MSS_Q((b & mask_md) == md);
            value = (b & mask_data);
            for (; span_.peek(b); span_.pop())
            {
                if ((b & mask_md) != md_data)
                    //Data ended already
                    break;
                value <<= 6;
                value |= (b & mask_data);
            }
            MSS_END();
        }

        ConstSpan span_;
    };

} } 

#endif
