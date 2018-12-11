#ifndef HEADER_gubg_t2_Span_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Span_hpp_ALREADY_INCLUDED

#include "gubg/t2/Types.hpp"

namespace gubg { namespace t2 { 

    class ConstSpan;

    class Span
    {
    public:
        Span(Byte *begin, Byte *end): begin_(begin), end_(end) {}
        Span(Span &rhs): begin_(rhs.begin_), end_(rhs.end_) {}

        bool empty() const {return begin_ == end_;}
        size_t size() const {return end_ - begin_;}

        Byte *begin() {return begin_;}
        Byte *end() {return end_;}

    private:
        friend class ConstSpan;

        Byte *begin_ = nullptr;
        Byte *end_ = nullptr;
    };

    class ConstSpan
    {
    public:
        ConstSpan(const Byte *begin, const Byte *end): begin_(begin), end_(end) {}
        ConstSpan(const ConstSpan &rhs): begin_(rhs.begin_), end_(rhs.end_) {}
        ConstSpan(const Span &rhs): begin_(rhs.begin_), end_(rhs.end_) {}

        void assign(const Byte *begin, const Byte *end)
        {
            begin_ = begin;
            end_ = end;
        }

        bool empty() const {return begin_ == end_;}
        size_t size() const {return end_ - begin_;}

        const Byte *begin() const {return begin_;}
        const Byte *end() const {return end_;}

        bool peek(Byte &b) const
        {
            if (begin_ == end_)
                return false;
            b = *begin_;
            return true;
        }

        bool pop(Byte &b)
        {
            if (begin_ == end_)
                return false;
            b = *begin_++;
            return true;
        }
        bool pop()
        {
            if (begin_ == end_)
                return false;
            begin_++;
            return true;
        }

    private:
        const Byte *begin_ = nullptr;
        const Byte *end_ = nullptr;
    };

} } 

#endif
