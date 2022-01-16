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

        bool pop_name(const std::string &expected_name)
        {
            MSS_BEGIN(bool);
            SavePoint sp(*this);
            Info info;
            MSS(pop_info_(info));
            Strange tag{info.tag};
            Strange name;
            MSS(tag.pop_until(name, ':') || tag.pop_all(name));
            MSS(name.str() == expected_name);
            sp.commit();
            path_.push_back(std::move(info));
            MSS_END();
        }
        bool pop_type(const std::string &expected_type)
        {
            MSS_BEGIN(bool);
            SavePoint sp(*this);
            Info info;
            MSS(pop_info_(info));
            Strange tag{info.tag};
            Strange dump;
            MSS(tag.pop_until(dump, ':'));
            MSS(tag.str() == expected_type);
            sp.commit();
            path_.push_back(std::move(info));
            MSS_END();
        }

        bool has_attr(const std::string &key)
        {
            MSS_BEGIN(bool);
            const auto &attrs = top_().attrs;
            auto it = attrs.find(key);
            MSS(it != attrs.end());
            MSS_END();
        }
        template <typename Value>
        bool get_attr(const std::string &key, Value &value)
        {
            return get_attr_<Value>(key, value);
        }

    private:
        template <typename Value>
        bool get_attr_(const std::string &key, Value &value, typename std::enable_if<std::is_same<Value, std::string>::value>::type * = 0)
        {
            MSS_BEGIN(bool);
            const auto &attrs = top_().attrs;
            auto it = attrs.find(key);
            MSS(it != attrs.end());
            value = it->second;
            MSS_END();
        }
        template <typename Value>
        bool get_attr_(const std::string &key, Value &value, typename std::enable_if<std::is_integral<Value>::value>::type * = 0)
        {
            MSS_BEGIN(bool);
            const auto &attrs = top_().attrs;
            auto it = attrs.find(key);
            MSS(it != attrs.end());
            value = std::stol(it->second);
            MSS_END();
        }
        template <typename Value>
        bool get_attr_(const std::string &key, Value &value, typename std::enable_if<std::is_floating_point<Value>::value>::type * = 0)
        {
            MSS_BEGIN(bool);
            const auto &attrs = top_().attrs;
            auto it = attrs.find(key);
            MSS(it != attrs.end());
            value = std::stod(it->second);
            MSS_END();
        }

        struct Info
        {
            std::string tag;
            Attrs attrs;
            Range block;
        };

        bool pop_info_(Info &info)
        {
            MSS_BEGIN(bool);
            auto &block = top_().block;
            MSS_Q(block.pop_tag(info.tag));
            block.pop_attrs(info.attrs);
            block.pop_block(info.block);
            MSS_END();
        }

        struct SavePoint
        {
            Range &dst;
            Range orig;
            bool committed = false;
            SavePoint(Reader &reader): dst(reader.top_().block), orig(dst) { }
            ~SavePoint()
            {
                if (committed)
                    return;
                dst = orig;
            }
            void commit() {committed = true;}
        };

        Range range_;

        Info &top_() {return path_.back();}
        std::list<Info> path_;
    };

} } 

#endif
