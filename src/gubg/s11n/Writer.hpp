#ifndef HEADER_gubg_s11n_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_Writer_hpp_ALREADY_INCLUDED

#include "gubg/s11n/Codes.hpp"
#include "gubg/s11n/StringAdapter.hpp"

namespace gubg { namespace s11n { 

    namespace details { 
        //push_back
        template <typename String>
        ReturnCode push_back(String &dst, char ch)
        {
            return dst.push_back(ch) ? ReturnCode::OK : ReturnCode::NoSpaceLeft;
        }

        //append
        template <typename String>
        ReturnCode append(String &dst, const char *src)
        {
            for (; *src; ++src)
                if (!dst.push_back(*src))
                    return ReturnCode::NoSpaceLeft;
            return ReturnCode::OK;
        }
        template <typename String>
        ReturnCode append(String &dst, const std::string &str)
        {
            for (const auto ch: str)
                if (!dst.push_back(ch))
                    return ReturnCode::NoSpaceLeft;
            return ReturnCode::OK;
        }
        template <typename String, typename Number>
        ReturnCode append(String &dst, Number number)
        {
            return append(dst, std::to_string(number));
        }
    } 

    enum class State
    {
        Attr, Open, Closed, 
    };

    template <typename Tag_, typename String_>
    class Writer
    {
    public:
        using Tag = Tag_;
        using String = String_;

        Writer(String &str): str_(str) {}

        template <typename Obj>
        ReturnCode typed(const Obj &obj)
        {
            MSS_BEGIN(ReturnCode);
            MSS_END();
        }

        template <typename Obj, typename Name>
        ReturnCode named(const Obj &obj, const Name &name)
        {
            MSS_BEGIN(ReturnCode);

            MSS(open_());

            MSS(details::push_back(str_, '['));
            MSS(details::append(str_, name));
            MSS(details::push_back(str_, ']'));

            path_.emplace_back();
            MSS(obj.write(*this));
            MSS(close_());
            path_.pop_back();

            MSS_END();
        }
        template <typename Obj, typename Name, typename Ftor>
        ReturnCode named(const Obj &obj, const Name &name, Ftor &&ftor)
        {
            MSS_BEGIN(ReturnCode);

            MSS(open_());

            MSS(details::push_back(str_, '['));
            MSS(details::append(str_, name));
            MSS(details::push_back(str_, ']'));

            path_.emplace_back();
            MSS(ftor(*this, obj));
            MSS(close_());
            path_.pop_back();

            MSS_END();
        }
        
        template <typename Key, typename Value>
        ReturnCode attr(const Key &key, const Value &value)
        {
            MSS_BEGIN(ReturnCode);

            MSS(!path_.empty());
            const auto &info = path_.back();

            MSS(info.state == State::Attr);

            MSS(details::push_back(str_, '('));
            MSS(details::append(str_, key));
            MSS(details::push_back(str_, ':'));
            MSS(details::append(str_, value));
            MSS(details::push_back(str_, ')'));

            MSS_END();
        }
        
        template <typename Value>
        ReturnCode text(const Value &value)
        {
            MSS_BEGIN(ReturnCode);

            MSS(!path_.empty());
            const auto &info = path_.back();

            MSS(open_());

            MSS(details::append(str_, value));

            MSS_END();
        }

    private:
        ReturnCode open_()
        {
            MSS_BEGIN(ReturnCode);
            if (!path_.empty())
            {
                auto &info = path_.back();
                if (info.state == State::Attr)
                {
                    info.state = State::Open;
                    MSS(details::push_back(str_, '{'));
                }
            }
            MSS_END();
        }
        ReturnCode close_()
        {
            MSS_BEGIN(ReturnCode);
            MSS(!path_.empty());
            auto &info = path_.back();
            if (info.state == State::Open)
            {
                info.state = State::Closed;
                MSS(details::push_back(str_, '}'));
            }
            MSS_END();
        }
        struct Info
        {
            State state = State::Attr;
        };
        using Path = std::vector<Info>;
        Path path_;
        StringAdapter<String> str_;
    };

} } 

#endif