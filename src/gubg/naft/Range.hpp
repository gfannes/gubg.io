#ifndef HEADER_gubg_naft_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Range_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
#include <map>

namespace gubg { namespace naft { 

    using Attrs = std::multimap<std::string, std::string>;

    class Range
    {
    public:
        using Position = Strange::Position;

        class Savepoint
        {
        public:
            Savepoint(Range &range): dst_(&range), orig_(range.strange_) {}
            Savepoint(Savepoint &&dying): dst_(dying.dst_), orig_(dying.orig_) { dying.dst_ = nullptr; }
            ~Savepoint() { if (!!dst_) dst_->strange_ = orig_; }
            void commit() {dst_ = nullptr;}
        private:
            Range *dst_ = nullptr;
            Strange orig_;
        };
        Savepoint savepoint() { return Savepoint(*this); }

        Range(){}
        Range(const std::string &str): strange_(str) {strip_();}

        bool empty() const {return strange_.empty();}
        std::string str() const {return strange_.str();}

        Position position() const {return strange_.position();}

        bool pop_tag(std::string &tag)
        {
            if (!strange_.pop_bracket(tag, "[]"))
                return false;
            descape_node_(tag);
            strip_();
            return true;
        }
        bool pop_tag(std::string &name, std::string &type)
        {
            if (!pop_tag(name))
                return false;
            descape_node_(name);
            const auto ix = name.find(':');
            if (ix != std::string::npos)
            {
                type = name.substr(ix+1);
                name.resize(ix);
            }
            return true;
        }
        //@warning: tag is not descaped
        bool pop_tag_unescaped(Strange &tag)
        {
            if (!strange_.pop_bracket(tag, "[]"))
                return false;
            strip_();
            return true;
        }
        bool pop_tag(const std::string &tag)
        {
            auto sp = strange_;
            if (!strange_.pop_bracket(tmp_, "[]"))
            {
                strange_ = sp;
                return false;
            }
            descape_node_(tmp_);
            if (tmp_ != tag)
            {
                strange_ = sp;
                return false;
            }
            strip_();
            return true;
        }

        bool pop_attr(std::string &key, std::string &value)
        {
            if (!strange_.pop_bracket(tmp_, "()"))
                return false;
            descape_attr_(tmp_);
            Strange kv{tmp_};

            if (kv.pop_until(key, ':'))
                kv.pop_all(value);
            else
            {
                kv.pop_all(key);
                value.clear();
            }

            strip_();

            return true;
        }
        void pop_attrs(Attrs &attrs)
        {
            Strange kv;
            std::string k,v;
            while (pop_attr(k, v))
                attrs.emplace(k, v);
        }
        Attrs pop_attrs()
        {
            Attrs attrs;
            pop_attrs(attrs);
            return attrs;
        }

        bool pop_block(Range &block)
        {
            if (!strange_.pop_bracket(block.strange_, "{}"))
                return false;
            block.strip_();
            strip_();
            return true;
        }
        bool pop_block(Strange &block)
        {
            if (!strange_.pop_bracket(block, "{}"))
                return false;
            strip_(block);
            strip_();
            return true;
        }

        std::string pop_text()
        {
            std::string res;
            strange_.pop_until(res, '[') || strange_.pop_all(res);
            descape_text_(res);
            return res;
        }
        bool pop_text_unescaped(Strange &strange)
        {
            return strange_.pop_until(strange, '[') || strange_.pop_all(strange);
        }

        bool pop_name(const std::string &name)
        {
            auto sp = strange_;
            if (!pop_tag(tmp_)
                || name != tmp_.substr(0, name.size()) //!tmp_.starts_with(name)
                || !(tmp_.size() == name.size() || tmp_[name.size()] == ':') //if tmp_ is longer than name, it should continue with ':'
                )
            {
                strange_ = sp;
                return false;
            }
            return true;
        }

        bool pop_type(const std::string &type)
        {
            auto sp = strange_;

            Strange dump;

            //Pop the type
            {
                if (!pop_tag(tmp_))
                {
                    strange_ = sp;
                    return false;
                }
                Strange tag{tmp_};
                if (!tag.pop_until(dump, ':') || !tag.pop_if(type) || !tag.empty())
                {
                    strange_ = sp;
                    return false;
                }
                strip_();
            }

            return true;
        }
        bool pop_type(const std::string &type, Range &rest)
        {
            auto sp = strange_;

            Strange dump;

            //Pop the type
            {
                if (!pop_tag(tmp_))
                {
                    strange_ = sp;
                    return false;
                }
                Strange tag{tmp_};
                if (!tag.pop_until(dump, ':') || !tag.pop_if(type) || !tag.empty())
                {
                    strange_ = sp;
                    return false;
                }
                strip_();
            }

            //Keep track of this position
            rest.strange_ = strange_;

            //Pop all attributes, we do not fully parse them
            while (strange_.pop_bracket(dump, "()"))
                strip_();

            //Pop the body, we do not fail if there is no body
            strange_.pop_bracket(strange_, "{}");

            //Adjust the length of rest.strange_
            if (!rest.strange_.diff_to(strange_))
            {
                assert(false);//Should not fail
                return false;
            }

            return true;
        }

    private:
        const std::string &whitespace_() const
        {
            static const std::string whitespace = " \t\n\r";
            return whitespace;
        }
        void strip_() {strange_.strip_left(whitespace_());}
        void strip_(Strange &strange) {strange.strip_left(whitespace_());}

        static void descape_node_(std::string &str) {descape_<'[', ']'>(str);}
        static void descape_attr_(std::string &str) {descape_<'(', ')'>(str);}
        static void descape_text_(std::string &str)
        {
            for (auto ix = 0u; ix < str.size(); ++ix)
            {
                if (str[ix] == '{' && (str.size()-ix >= 3) && (str[ix+2] == '}'))
                {
                    switch (str[ix+1])
                    {
                        case '<': str[ix] = '{'; str.erase(ix+1, 2); break;
                        case '>': str[ix] = '}'; str.erase(ix+1, 2); break;
                        case '(': str[ix] = '['; str.erase(ix+1, 2); break;
                        case ')': str[ix] = ']'; str.erase(ix+1, 2); break;
                        default: break;
                    }
                }
            }
        }

        template <char o, char c>
        static void descape_(std::string &str)
        {
            for (auto ix = 0u; ix < str.size(); ++ix)
            {
                if (str[ix] == o && (str.size()-ix >= 3) && (str[ix+2] == c))
                {
                    switch (str[ix+1])
                    {
                        case '<': str[ix] = o; str.erase(ix+1, 2); break;
                        case '>': str[ix] = c; str.erase(ix+1, 2); break;
                        default: break;
                    }
                }
                else if (str[ix] == '{' && (str.size()-ix >= 3) && (str[ix+2] == '}'))
                {
                    switch (str[ix+1])
                    {
                        case '<': str[ix] = '{'; str.erase(ix+1, 2); break;
                        case '>': str[ix] = '}'; str.erase(ix+1, 2); break;
                        default: break;
                    }
                }
            }
        }

        Strange strange_;
        std::string tmp_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Range &range)
    {
        os << range.str();
        return os;
    }
} } 

#endif
