#ifndef HEADER_gubg_Strange_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Strange_hpp_ALREADY_INCLUDED

#include "gubg/parse/numbers/Integer.hpp"
#include <cstdlib>
#include <cassert>
#include <ostream>
#include <cstring>
#include <cstdint>
#include <memory>

//TODO: the methods that take a Strange& as argument are currently not correct when this argument
//is the same as the this pointer

namespace gubg {

    class Strange
    {
    public:
        struct Position
        {
            size_t ix = 0;//zero-based
            size_t line = 0;//zero-based
            size_t column = 0;//zero-based
        };

        Strange();
        Strange(const std::string &);
        Strange(const char *cstr);
        Strange(const char *buffer, size_t len);
        Strange(std::string &&);

        Strange(const Strange &);
        Strange(Strange &&);

        Strange &operator=(const Strange &);
        Strange &operator=(Strange &&);
        Strange &operator=(const std::string &);
        Strange &operator=(std::string &&);

        void swap(Strange &rhs)
        {
            std::swap(data_, rhs.data_);
            std::swap(s_, rhs.s_);
            std::swap(l_, rhs.l_);
        }

        bool empty() const;
        size_t size() const;
        std::string str() const;
        char front() const;
        char back() const;
        void clear();

        bool contains(char ch) const;

        bool strip(char ch);
        bool strip(const std::string &chars);

        //Return true if !res.empty()
        bool pop_all(Strange &res);
        bool pop_all(std::string &res);

        //Does not pop ch or str
        bool pop_to(Strange &res, const char ch);
        bool pop_to(Strange &res, const std::string &str);
        //Pops ch too, set inclusive to true if you want ch to be included in res
        bool pop_until(Strange &res, const char ch, bool inclusive = false);
        bool pop_until(std::string &res, const char ch, bool inclusive = false);
        bool pop_until(Strange &res, const std::string &str, bool inclusive = false);
        bool pop_until(std::string &res, const std::string &str, bool inclusive = false);

        bool pop_bracket(Strange &res, const std::string &oc);
        bool pop_bracket(std::string &res, const std::string &oc);

        //this and strange are assumed to be related and have the same end
        bool diff_to(const Strange &strange);

        bool pop_decimal(long &res);
        template <typename Int>
        bool pop_decimal(Int &i)
        {
            long l;
            if (!pop_decimal(l))
                return false;
            i = l;
            return true;
        }
        bool pop_float(double &res);
        bool pop_float(float &res);

        bool pop_if(const char ch);
        bool pop_back_if(const char ch);
        bool pop_front();
        bool pop_back();
        bool pop_char(char &ch);

        bool pop_string(std::string &, size_t nr);
        bool pop_if(const std::string &);

        bool pop_line(Strange &line);

        void ltrim(const char ch);

        bool pop_lsb(std::uint8_t &);
        bool pop_lsb(std::uint16_t &);
        bool pop_lsb(std::uint32_t &);
        bool pop_lsb(std::uint64_t &);
        bool pop_lsb(std::int8_t &);
        bool pop_lsb(std::int16_t &);
        bool pop_lsb(std::int32_t &);
        bool pop_lsb(std::int64_t &);

        bool pop_msb(std::uint8_t &);
        bool pop_msb(std::uint16_t &);
        bool pop_msb(std::uint32_t &);
        bool pop_msb(std::uint64_t &);
        bool pop_msb(std::int8_t &);
        bool pop_msb(std::int16_t &);
        bool pop_msb(std::int32_t &);
        bool pop_msb(std::int64_t &);

        bool pop_count(size_t nr);
        bool pop_count(Strange &, size_t nr);

        bool pop_raw(char *dst, size_t nr);

        template <typename Ftor>
        void each_split(char delim, Ftor &&ftor)
        {
            Strange part;
            while (pop_until(part, delim) || pop_all(part))
                ftor(part);
        }

        Position position() const;

    private:
        template <typename T>
        bool pop_lsb_(T &);
        template <typename T>
        bool pop_msb_(T &);
        bool invariants_() const;
        void forward_(const size_t nr);
        void shrink_(const size_t nr);

        std::shared_ptr<const std::string> data_;
        const char *s_;
        size_t l_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Strange &strange)
    {
        os << strange.str();
        return os;
    }
}

#endif
