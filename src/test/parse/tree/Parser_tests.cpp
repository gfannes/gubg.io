#include "catch.hpp"
#include "gubg/debug.hpp"
/* #include "gubg/parse/tree/Parser.hpp" */
#include <string>
#include <vector>

namespace gubg { namespace tree { 

    struct EndOfDocument { };

    enum State {Text, Tag, Attr, Open, Close};
    std::ostream &operator<<(std::ostream &os, State s)
    {
        switch (s)
        {
            case Text: os << "Text"; break;
            case Tag: os << "Tag"; break;
            case Attr: os << "Attr"; break;
            case Open: os << "Open"; break;
            case Close: os << "Close"; break;
            default: os << "Unknown state " << (int)s; break;
        }
        return os;
    }

    template <typename Receiver>
    class Parser_crtp
    {
    private:
        constexpr static const char *logns = "tree::Parser";

    public:
        void process(const std::string &content)
        {
            for (auto ch: content)
                process(ch);
            process(EndOfDocument());
        }

        void process(char ch)
        {
            S(logns);L(C(ch));
            switch (state_)
            {
                case Text:
                    switch (ch)
                    {
                        case '[':
                            change_state_<Tag>();
                            break;
                    }
                    break;
                case Tag:
                    switch (ch)
                    {
                        case ']':
                            change_state_<Text>();
                            break;
                        default:
                            tag_.push_back(ch);
                            break;
                    }
                    break;
                case Attr:
                    break;
                case Open:
                    break;
                case Close:
                    break;
            }
        }
        void process(EndOfDocument)
        {
            if (tag_is_open_)
            {
                receiver_().tree_close();
                tag_is_open_ = false;
            }
            switch (state_)
            {
                case Text:
                    break;
                case Tag:
                    break;
                case Attr:
                    break;
                case Open:
                    break;
                case Close:
                    break;
            }
            for (; scope_level_ > 0; --scope_level_)
            {
                receiver_().tree_close();
            }
        }

    private:
        Receiver &receiver_() {return static_cast<Receiver&>(*this);}

        template <State WantedState>
        void change_state_()
        {
            if (state_ == WantedState)
                return;

            S(logns);
            L(C(state_)C(WantedState));

            //Exit
            switch (state_)
            {
                case Text:
                    if (!text_.empty())
                        receiver_().tree_text(text_);
                    text_.clear();
                    break;
                case Tag:
                    if (!tag_.empty())
                        receiver_().tree_open_tag(tag_);
                    tag_is_open_ = true;
                    break;
            }

            state_ = WantedState;

            //Enter
            switch (state_)
            {
                case Tag:
                    if (tag_is_open_)
                    {
                        receiver_().tree_close();
                        tag_is_open_ = false;
                    }
                    tag_.clear();
                    break;
            }
        }

        State state_ = Text;
        bool tag_is_open_ = false;
        unsigned int bracket_level_ = 0;
        unsigned int scope_level_ = 0;
        std::string text_;
        std::string tag_;
        std::string key_;
        std::string value_;
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
    private:
    };
} 

TEST_CASE("tree::Parser tests", "[ut][tree]")
{
    std::string content;
    std::vector<std::string> wanted;

    SECTION("empty") { }
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

    Parser parser;
    parser.process(content);

    REQUIRE(parser.result == wanted);
}
