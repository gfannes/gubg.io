#include "catch.hpp"
#include "gubg/debug.hpp"
/* #include "gubg/parse/tree/Parser.hpp" */
#include <string>
#include <vector>
#include <cassert>

namespace gubg { namespace tree { 

    enum class State {Start, Text, Tag, Attr, Open, Close, Stop};
    std::ostream &operator<<(std::ostream &os, State s)
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
        constexpr static const char *logns = nullptr;//"tree::Parser";

    public:
        Parser_crtp()
        {
            change_state_<State::Text>();
        }

        void process(const std::string &content)
        {
            for (auto ch: content)
                (this->*process_)(ch);
            end_of_document_();
        }

    private:
        Receiver &receiver_() {return static_cast<Receiver&>(*this);}

        void nothing_() {}
        void nothing_(char ch) {}

        void (Self::*enter_)() = nullptr;
        void (Self::*process_)(char ch) = nullptr;
        void (Self::*exit_)() = &Self::nothing_;

        unsigned int bracket_level_ = 0;

        //State::Text
        std::string text_;
        void text_enter_()
        {
            S(logns);
            text_.clear();
            bracket_level_ = 0;
        }
        void text_exit_()
        {
            S(logns);
            if (!text_.empty())
                receiver_().tree_text(text_);
        }
        void text_process_(char ch)
        {
            S(logns);
            //We only react if the we are not within <> brackets
            if (bracket_level_ == 0)
            {
                switch (ch)
                {
                    case '[':
                        change_state_<State::Tag>();
                        return;
                        break;
                    case '(':
                        change_state_<State::Attr>();
                        return;
                        break;
                    case '{':
                        if (tag_is_open_)
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
            switch (ch)
            {
                case '<':
                    ++bracket_level_;
                    break;
                case '>':
                    --bracket_level_;
                    break;
            }
            text_.push_back(ch);
        }

        //State::Tag
        std::string tag_;
        void tag_enter_()
        {
            S(logns);
            if (tag_is_open_)
            {
                receiver_().tree_close();
                tag_is_open_ = false;
            }
            tag_.clear();
            bracket_level_ = 0;
        }
        void tag_exit_()
        {
            S(logns);
            receiver_().tree_open_tag(tag_);
            tag_is_open_ = true;
        }
        void tag_process_(char ch)
        {
            S(logns);
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
        std::string key_;
        std::string value_;
        std::string *kv_;
        void attr_enter_()
        {
            key_.clear();
            value_.clear();
            kv_ = &key_;
            bracket_level_ = 0;
        }
        void attr_exit_()
        {
            receiver_().tree_attr(key_, value_);
        }
        void attr_process_(char ch)
        {
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
            ++scope_level_;
            tag_is_open_ = false;
            change_state_<State::Text>();
        }
        void close_enter_()
        {
            assert(scope_level_ > 0);
            --scope_level_;
            receiver_().tree_close();
            change_state_<State::Text>();
        }

        template <State WantedState>
        void change_state_()
        {
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
            change_state_<State::Text>();
            if (tag_is_open_)
            {
                receiver_().tree_close();
                tag_is_open_ = false;
            }

            while (scope_level_ > 0)
                change_state_<State::Close>();

            change_state_<State::Stop>();
        }

        State state_ = State::Start;
        bool tag_is_open_ = false;
    };

} } 

namespace  { 
    class Parser: public gubg::tree::Parser_crtp<Parser>
    {
    public:
        std::vector<std::string> result;

        void tree_text(const std::string &text)
        {
            result.push_back("text:"+text);
        }
        void tree_open_tag(const std::string &tag)
        {
            result.push_back("open:"+tag);
        }
        void tree_close()
        {
            result.push_back("close");
        }
        void tree_attr(const std::string &key, const std::string &value)
        {
            result.push_back("attr:"+key+"=>"+value);
        }
    private:
    };
} 

TEST_CASE("tree::Parser tests", "[ut][tree2]")
{
    std::string content;
    std::vector<std::string> wanted;

    SECTION("empty") { }
    SECTION("text")
    {
        content = "text";
        wanted.push_back("text:text");
    }
    SECTION("one tag")
    {
        content = "[tag]";
        wanted.push_back("open:tag");
        wanted.push_back("close");
    }
    SECTION("two tags")
    {
        content = "[tag_a][tag_b]";
        wanted.push_back("open:tag_a");
        wanted.push_back("close");
        wanted.push_back("open:tag_b");
        wanted.push_back("close");
    }
    SECTION("tag + 2 attr")
    {
        content = "[tag](a:0)(b)(:c)(a:1)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a=>0");
        wanted.push_back("attr:b=>");
        wanted.push_back("attr:=>c");
        wanted.push_back("attr:a=>1");
        wanted.push_back("close");
    }
    SECTION("2 x (tag + attr)")
    {
        content = "[tag0](a:0)[tag1](b:1)";
        wanted.push_back("open:tag0");
        wanted.push_back("attr:a=>0");
        wanted.push_back("close");
        wanted.push_back("open:tag1");
        wanted.push_back("attr:b=>1");
        wanted.push_back("close");
    }
    SECTION("nested")
    {
        content = "[tag0]{[tag1]}}";
        wanted.push_back("open:tag0");
        wanted.push_back("open:tag1");
        wanted.push_back("close");
        wanted.push_back("close");
        wanted.push_back("text:}");
    }
    SECTION("tag with []")
    {
        content = "[tag[]]";
        wanted.push_back("open:tag[]");
        wanted.push_back("close");
    }
    SECTION("attr with ()")
    {
        content = "[tag](a():0)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a()=>0");
        wanted.push_back("close");
    }
    SECTION("key with :")
    {
        content = "[tag](a:b:c)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a=>b:c");
        wanted.push_back("close");
    }
    SECTION("comment")
    {
        content = "[tag](a:0)<(b:1)>(c:2)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a=>0");
        wanted.push_back("text:<(b:1)>");
        wanted.push_back("attr:c=>2");
        wanted.push_back("close");
    }
    SECTION("{ before a tag")
    {
        content = "{}";
        wanted.push_back("text:{}");
    }

    Parser parser;
    parser.process(content);

    REQUIRE(parser.result == wanted);
}
