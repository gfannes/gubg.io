#ifndef HEADER_gubg_hr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_hr_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <gubg/hex.hpp>
#include <sstream>
#include <vector>
#include <array>
#include <tuple>
#include <set>

#ifdef CHR
#error CHR already defined
#endif
#define CHR(name) "{" #name ":" << gubg::hr(name) << "}"

namespace gubg { 

    inline std::string hr(const double &d)
    {
        return std::to_string(d);
    }
    inline std::string hr(const std::string &str)
    {
        std::ostringstream oss;
        oss << "\"" << str << "\"";
        return oss.str();
    }
    template <typename T>
        std::string hr(const T &t)
        {
            std::ostringstream oss;
            oss << t;
            return oss.str();
        }
    template <typename T>
        std::string hr_hex(const T &t)
        {
            std::ostringstream oss;
            oss << hex(t);
            return oss.str();
        }
    template <typename T>
        std::string hr(const std::vector<T> &vec)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (const auto &v: vec)
                oss << (skipComma() ? "" : ", ") << hr(v);
            oss << '}';
            return oss.str();
        }
    template <typename T>
        std::string hr_hex(const std::vector<T> &vec)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (const auto &v: vec)
                oss << (skipComma() ? "" : ", ") << hr_hex(v);
            oss << '}';
            return oss.str();
        }
    template <typename T>
        std::string hr(const std::set<T> &vec)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (const auto &v: vec)
                oss << (skipComma() ? "" : ", ") << hr(v);
            oss << '}';
            return oss.str();
        }

    template <typename T, size_t N>
        std::string hr(const std::array<T, N> &ary)
        {
            std::ostringstream oss;
            oss << '{';
            OnlyOnce skipComma;
            for (const auto &v: ary)
                oss << (skipComma() ? "" : ", ") << hr(v);
            oss << '}';
            return oss.str();
        }

    namespace details { 
        template <size_t IX>
            struct StreamTupleValue
            {
                template <typename Tuple>
                    static void stream(std::ostream &os, const Tuple &t)
                    {
                        StreamTupleValue<IX-1>::stream(os, t);
                        os << hr(std::get<IX-1>(t));
                        constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
                        if (IX == tuple_size)
                            os << "}";
                        else
                            os << ", ";
                    }
            };
        template <>
            struct StreamTupleValue<0>
            {
                template <typename Tuple>
                    static void stream(std::ostream &os, const Tuple &t)
                    {
                        os << "{";
                    }
            };
    } 
    template <typename... Ts>
        std::string hr(const std::tuple<Ts...> &t)
        {
            typedef std::tuple<Ts...> Tuple;
            std::ostringstream oss;
            constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
            details::StreamTupleValue<tuple_size>::stream(oss, t);
            return oss.str();
        }

} 

#endif
