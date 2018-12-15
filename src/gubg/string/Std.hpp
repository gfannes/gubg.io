#ifndef HEADER_gubg_string_Std_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_Std_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace string { 

    class Std
    {
    public:
        using value_type = char;

        Std(std::string &str): str_(str) {}

        void clear() {str_.clear();}

        bool empty() const {return str_.empty();}
        size_t size() const {return str_.size();}

        const value_type &operator[](size_t ix) const {return str_[ix];}
        value_type &operator[](size_t ix) {return str_[ix];}

        const value_type *data() const {return str_.data();}
        value_type *data() {return str_.data();}

        bool add(value_type ch)
        {
            str_.push_back(ch);
            return true;
        }

        value_type *grow(size_t nr)
        {
            const auto s = size();
            str_.resize(s+nr);
            return str_.data()+s;
        }
        value_type *grow(size_t nr, value_type ch)
        {
            const auto s = size();
            str_.resize(s+nr, ch);
            return str_.data()+s;
        }

        bool shrink(size_t nr)
        {
            const auto s = size();
            if (nr > s)
                return false;
            str_.resize(s-nr);
            return true;
        }

    private:
        std::string &str_;
    };

} } 

#endif
