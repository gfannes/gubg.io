#ifndef HEADER_gubg_parse_naft_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_naft_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/naft/Codes.hpp"
#include "gubg/Strange.hpp"
#include "gubg/mss.hpp"
#include <string>
#include <map>

namespace gubg { namespace parse { namespace naft { 

    enum class State {Start, Text, Tag, Attr, Open, Close, Stop};
    inline std::ostream &operator<<(std::ostream &os, State s)
    {
        switch (s)
        {
            case State::Text: os << "Text"; break;
            case State::Tag: os << "Tag"; break;
            case State::Attr: os << "Attr"; break;
            case State::Open: os << "Open"; break;
            case State::Close: os << "Close"; break;
            default: os << "Unknown state " << (int)s; break;
        }
        return os;
    }

    template <typename Receiver>
    class Parser_crtp
    {
    private:
        using Self = Parser_crtp<Receiver>;
        constexpr static const char *logns = nullptr;//"naft::Parser";

    public:
        using Tag = std::string;
        using Key = std::string;
        using Value = std::string;
        using Text = std::string;

        Parser_crtp()
        {
            reset();
        }

        bool process(const std::string &content)
        {
            MSS_BEGIN(bool);
            for (auto ch: content)
            {
                /* std::cout << "(" << ch << ")" << std::endl; */
                (this->*process_)(ch);
                if (!ok_)
                    break;
            }
            end_of_document_();
            MSS(ok_);
            MSS_END();
        }

        bool process(char ch)
        {
            MSS_BEGIN(bool);
            (this->*process_)(ch);
            MSS(ok_);
            MSS_END();
        }
        bool stop()
        {
            MSS_BEGIN(bool);
            end_of_document_();
            MSS(ok_);
            MSS_END();
        }

        void reset()
        {
            ok_ = true;
            exit_ = &Self::nothing_;
            change_state_<State::Text>();
        }

    private:
        Receiver &receiver_() {return static_cast<Receiver&>(*this);}

        bool ok_ = true;

        void nothing_() {}
        void nothing_(char ch) {}

        void (Self::*enter_)() = nullptr;
        void (Self::*process_)(char ch) = nullptr;
        void (Self::*exit_)() = &Self::nothing_;

        unsigned int bracket_level_ = 0;
        unsigned int fence_depth_ = 0;

        //State::Text
        Text text_;
        void text_enter_()
        {
            S(logns);
            text_.resize(0);
            fence_depth_ = 0;
            bracket_level_ = 0;
        }
        void text_exit_()
        {
            S(logns);
            if (!text_.empty())
                ok_ = ok_ && receiver_().naft_text(text_);
        }
        void text_process_(char ch)
        {
            S(logns);L(ch);

            if (fence_depth_ == 0)
            {
                switch (ch)
                {
                    case '`':
                        ++bracket_level_;
                        break;
                    default:
                        fence_depth_ = bracket_level_;
                        break;
                }
            }
            else
            {
                switch (ch)
                {
                    case '`':
                        --bracket_level_;
                        if (bracket_level_ == 0)
                            fence_depth_ = 0;
                        break;
                    default:
                        bracket_level_ = fence_depth_;
                        break;
                }
            }

            //We only react if the we are not within a ```-fenced block. The number of ``` is variable, but should match.
            if (bracket_level_ == 0)
            {
                switch (ch)
                {
                    case '[':
                        change_state_<State::Tag>();
                        return;
                        break;
                    case '(':
                        if (attr_allowed_)
                        {
                            change_state_<State::Attr>();
                            return;
                        }
                        break;
                    case '{':
                        if (attr_allowed_)
                        {
                            change_state_<State::Open>();
                            return;
                        }
                        //Probably a { before a tag
                        break;
                    case '}':
                        if (scope_level_ > 0)
                        {
                            change_state_<State::Close>();
                            return;
                        }
                        //Probably incorrect nesting, we treat this as text
                        break;
                }
            }
            text_.push_back(ch);
        }

        //State::Tag
        Tag tag_;
        void tag_enter_()
        {
            S(logns);
            if (attr_allowed_)
            {
                attr_allowed_ = false;
                ok_ = ok_ && receiver_().naft_attr_done();
                ok_ = ok_ && receiver_().naft_node_close();
            }
            tag_.resize(0);
            bracket_level_ = 0;
        }
        void tag_exit_()
        {
            S(logns);
            ok_ = ok_ && receiver_().naft_node_open(tag_);
            attr_allowed_ = true;
        }
        void tag_process_(char ch)
        {
            S(logns);L(ch);
            switch (ch)
            {
                case ']':
                    if (bracket_level_ == 0)
                    {
                        change_state_<State::Text>();
                        return;
                    }
                    --bracket_level_;
                    break;
                case '[':
                    ++bracket_level_;
                    break;
            }
            tag_.push_back(ch);
        }

        //State::Attr
        Key key_;
        Value value_;
        std::string *kv_;
        void attr_enter_()
        {
            S(logns);
            key_.resize(0);
            value_.resize(0);
            kv_ = &key_;
            bracket_level_ = 0;
        }
        void attr_exit_()
        {
            S(logns);
            ok_ = ok_ && receiver_().naft_attr(key_, value_);
        }
        void attr_process_(char ch)
        {
            S(logns);L(ch);
            switch (ch)
            {
                case ':':
                    if (kv_ == &key_)
                    {
                        kv_ = &value_;
                        return;
                    }
                    break;
                case '(':
                    ++bracket_level_;
                    break;
                case ')':
                    if (bracket_level_ == 0)
                    {
                        change_state_<State::Text>();
                        return;
                    }
                    --bracket_level_;
                    break;
            }
            kv_->push_back(ch);
        }

        //State::Open/Close
        unsigned int scope_level_ = 0;
        void open_enter_()
        {
            S(logns);
            ++scope_level_;
            assert(attr_allowed_);
            attr_allowed_ = false;
            ok_ = ok_ && receiver_().naft_attr_done();
            change_state_<State::Text>();
        }
        void close_enter_()
        {
            S(logns);
            assert(scope_level_ > 0);
            --scope_level_;
            if (attr_allowed_)
            {
                attr_allowed_ = false;
                ok_ = ok_ && receiver_().naft_attr_done();
                ok_ = ok_ && receiver_().naft_node_close();
            }
            ok_ = ok_ && receiver_().naft_node_close();
            change_state_<State::Text>();
        }

        template <State WantedState>
        void change_state_()
        {
            S(logns);

            if (state_ == WantedState)
                return;

            (this->*exit_)();

            state_ = WantedState;

            switch (WantedState)
            {
                case State::Text:
                    enter_ = &Self::text_enter_;
                    process_ = &Self::text_process_;
                    exit_ = &Self::text_exit_;
                    break;
                case State::Tag:
                    enter_ = &Self::tag_enter_;
                    process_ = &Self::tag_process_;
                    exit_ = &Self::tag_exit_;
                    break;
                case State::Attr:
                    enter_ = &Self::attr_enter_;
                    process_ = &Self::attr_process_;
                    exit_ = &Self::attr_exit_;
                    break;
                case State::Open:
                    enter_ = &Self::open_enter_;
                    process_ = &Self::nothing_;
                    exit_ = &Self::nothing_;
                    break;
                case State::Close:
                    enter_ = &Self::close_enter_;
                    process_ = &Self::nothing_;
                    exit_ = &Self::nothing_;
                    break;
                case State::Stop:
                    enter_ = &Self::nothing_;
                    process_ = &Self::nothing_;
                    exit_ = &Self::nothing_;
                    break;
            }

            (this->*enter_)();
        }

        void end_of_document_()
        {
            S(logns);

            change_state_<State::Text>();

            while (scope_level_ > 0)
                change_state_<State::Close>();

            change_state_<State::Stop>();

            if (attr_allowed_)
            {
                attr_allowed_ = false;
                ok_ = ok_ && receiver_().naft_attr_done();
                ok_ = ok_ && receiver_().naft_node_close();
            }
        }

        State state_ = State::Start;
        bool attr_allowed_ = false;
    };

} } } 

#endif
