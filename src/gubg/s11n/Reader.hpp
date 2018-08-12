#ifndef HEADER_gubg_s11n_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_Reader_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
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
        inline void assign(float &dst, const std::string &src) { dst = std::stof(src); }
        inline void assign(double &dst, const std::string &src) { dst = std::stod(src); }
    } 

    class Reader
    {
    public:
        Reader(const std::string &message): range_(message) {}

        std::string tag() {return tag_;}

        bool empty() const {return range_.empty();}

        template <typename Tag, typename Ftor>
        bool operator()(const Tag &wanted_tag, Ftor &&ftor)
        {
            MSS_BEGIN(bool);

            auto sp = range_.savepoint();

            Strange tag;
            if (!range_.pop_tag(tag))
            {
                L("No naft tag found");
                return false;
            }

            if (!details::is_same(wanted_tag, tag))
            {
                L("Tags are different");
                return false;
            }

            Reader rdr;
            rdr.tag_ = tag.str();
            range_.pop_attrs(rdr.attrs_);
            range_.pop_block(rdr.range_);

            MSS(ftor(rdr));

            sp.commit();

            MSS_END();
        }

        template <typename Tag, typename Obj>
        bool object(const Tag &tag, Obj &obj)
        {
            return operator()(tag, [&](Reader &r){return obj.read(r);});
        }
        template <typename Tag, typename Obj, typename Info>
        bool object(const Tag &tag, Obj &obj, const Info &info)
        {
            return operator()(tag, [&](Reader &r){return obj.read(r, info);});
        }

        template <typename Value>
        bool attr(const std::string &key, Value &value)
        {
            auto it = attrs_.find(key);
            if (it == attrs_.end())
                return false;
            details::assign(value, it->second);
            attrs_.erase(it);
            return true;
        }
        template <typename Ftor>
        bool attr_ftor(const std::string &key, Ftor &&ftor)
        {
            auto it = attrs_.find(key);
            if (it == attrs_.end())
                return false;
            if (!ftor(it->second))
                return false;
            attrs_.erase(it);
            return true;
        }
        template <typename Ftor>
        void each_attr(Ftor &&ftor)
        {
            for (const auto &p: attrs_)
                ftor(p.first, p.second);
            attrs_.clear();
        }

        std::string text()
        {
            return range_.pop_text();
        }
        bool text(Strange &strange)
        {
            return range_.pop_text(strange);
        }

        void stream(std::ostream &os) const
        {
            for (const auto &p: attrs_)
                os << "(" << p.first << ":" << p.second << ")" << std::endl;
            os << "{" << range_.str() << "}" << std::endl;
        }

    private:
        Reader(){}

        std::string tag_;
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
