#ifndef HEADER_gubg_s11n_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_Reader_hpp_ALREADY_INCLUDED

#include "gubg/naft/Range.hpp"
#include "gubg/mss.hpp"
#include <string>
#include <list>
#include <sstream>

namespace gubg { namespace s11n { 

    namespace details { 
        template <typename Name>
        bool is_same(const Name &w, const Strange &a)
        {
            std::ostringstream oss; oss << w;
            return a.str() == oss.str();
        }
        inline bool is_same(const std::string &w, const Strange &a) { return a.str() == w; }
        inline bool is_same(const char *w, const Strange &a) { return a.str() == w; }

        inline void assign(std::string &dst, const std::string &src) { dst = src; }
        inline void assign(int &dst, const std::string &src) { dst = std::stoi(src); }
        inline void assign(unsigned int &dst, const std::string &src) { dst = std::stoul(src); }
    } 

    class Reader
    {
    public:
        Reader(const std::string &message): range_(message) {}

        template <typename Name, typename Ftor>
        bool named(const Name &wanted_name, Ftor &&ftor)
        {
            MSS_BEGIN(bool);

            auto sp = range_.savepoint();

            Strange tag;
            if (!range_.pop_tag(tag))
            {
                L("No naft tag found");
                return false;
            }
            Strange actual_name;
            if (!(tag.pop_until(actual_name, ':') || tag.pop_all(actual_name)))
            {
                L("No name found");
                return false;
            }

            if (!details::is_same(wanted_name, actual_name))
            {
                L("Names are different");
                return false;
            }

            Reader rdr;
            range_.pop_attrs(rdr.attrs_);
            range_.pop_block(rdr.range_);

            MSS(ftor(rdr));

            sp.commit();

            MSS_END();
        }

        template <typename Value>
        bool attr(Value &value, const std::string &key)
        {
            auto it = attrs_.find(key);
            if (it == attrs_.end())
                return false;
            details::assign(value, it->second);
            attrs_.erase(it);
            return true;
        }

        std::string text()
        {
            return range_.pop_text();
        }

        void stream(std::ostream &os) const
        {
            for (const auto &p: attrs_)
                os << "(" << p.first << ":" << p.second << ")" << std::endl;
            os << "{" << range_.str() << "}" << std::endl;
        }

    private:
        Reader(){}

        naft::Attrs attrs_;
        naft::Range range_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Reader &rdr)
    {
        rdr.stream(os);
        return os;
    }

} } 

#endif
