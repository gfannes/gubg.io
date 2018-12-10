#ifndef HEADER_gubg_string_Buffer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_Buffer_hpp_ALREADY_INCLUDED

#include "gubg/std/algorithm.hpp"
#include "gubg/debug.hpp"

namespace gubg { namespace string { 

    template <typename Char>
    class Buffer
    {
    public:
        using value_type = Char;

        Buffer(value_type *begin, value_type *end): begin_(begin), current_(begin), end_(end) {}

        bool empty() const {return begin_ == current_;}
        size_t size() const {return current_ - begin_;}

        const value_type &operator[](size_t ix) const {return begin_[ix];}
        value_type &operator[](size_t ix) {return begin_[ix];}

        const value_type *data() const {return begin_;}
        value_type *data() {return begin_;}

        bool add(value_type ch)
        {
            if (current_ == end_)
                return false;
            *current_++ = ch;
            return true;
        }

        value_type *grow(size_t nr)
        {
            const auto free = end_ - current_;
            if (free < nr)
                return nullptr;
            auto ptr = current_;
            current_ += nr;
            return ptr;
        }
        value_type *grow(size_t nr, value_type ch)
        {
            const auto free = end_ - current_;
            if (free < nr)
                return nullptr;
            auto ptr = current_;
            current_ += nr;
            std::fill(ptr, current_, ch);
            return ptr;
        }

        bool shrink(size_t nr)
        {
            const auto s = size();
            if (nr > s)
                return false;
            current_ -= nr;
            return true;
        }

    private:
        value_type * begin_ = nullptr;
        value_type * current_ = nullptr;
        value_type * end_ = nullptr;
    };

} } 

#endif
