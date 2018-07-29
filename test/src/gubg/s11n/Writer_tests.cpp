#include "catch.hpp"
#include "gubg/s11n/Writer.hpp"
#include <optional>
#include <iostream>
using namespace gubg;

namespace  { 
    using RC = s11n::ReturnCode;

    class String
    {
    public:
        String(size_t max): max_(max) {}

        const std::string &str() const {return str_;}

        bool push_back(char ch)
        {
            if (str_.size() >= max_)
                return false;
            str_.push_back(ch);
            return true;
        }

    private:
        const size_t max_;
        std::string str_;
    };

    class Array
    {
    public:
        void push_back(int v) {values_.push_back(v);}

        template <typename Writer>
        bool write(Writer &writer) const
        {
            MSS_BEGIN(bool);
            MSS(writer.attr("size", values_.size()));
            size_t ix = 0;
            for (const auto &value: values_)
            {
                MSS(writer.named(value, ix, [](Writer &writer, int value){return writer.attr("value", value);}));
                ++ix;
            }
            MSS_END();
        }
    private:
        std::vector<int> values_;
    };
    class Text
    {
    public:
        void set(const std::string &str) {str_ = str;}

        template <typename Writer>
        bool write(Writer &writer) const
        {
            MSS_BEGIN(bool);
            MSS(writer.attr("encoding", "as_is"));
            MSS(writer.text(str_));
            MSS_END();
        }
    private:
        std::string str_;
    };

    struct MyTag { };
} 

TEST_CASE("s11n::Writer tests", "[ut][s11n][Writer]")
{
    using Writer = s11n::Writer<MyTag, String>;

    struct Scn
    {
        size_t str_size = 0;
        std::optional<Array> array;
        std::optional<Text> text;
        bool as_typed = true;
    };
    struct Exp
    {
        RC rc = RC::OK;
        std::string output;
    };
    Scn scn;
    Exp exp;

    SECTION("default")
    {
    }
    SECTION("Array")
    {
        scn.array.emplace();
        SECTION("as typed")
        {
            scn.as_typed = true;
        }
        SECTION("as named")
        {
            scn.as_typed = false;
            SECTION("empty, as named")
            {
                exp.output = "[array](size:0)";
            }
            SECTION("size 3, as named")
            {
                scn.array->push_back(0);
                scn.array->push_back(1);
                scn.array->push_back(2);
                exp.output = "[array](size:3){[0](value:0)[1](value:1)[2](value:2)}";
            }
        }
    }
    SECTION("Text")
    {
        scn.text.emplace();
        SECTION("as typed")
        {
            scn.as_typed = true;
        }
        SECTION("as named")
        {
            scn.as_typed = false;
            SECTION("empty, as named")
            {
                exp.output = "[text](encoding:as_is){}";
            }
            SECTION("size 3, as named")
            {
                scn.text->set("abc");
                exp.output = "[text](encoding:as_is){abc}";
            }
        }
    }
    scn.str_size = exp.output.size();

    String str(scn.str_size);
    Writer writer(str);

    RC rc = RC::OK;
    if (scn.array)
    {
        if (scn.as_typed)
        {
            AGG(rc, writer.typed(*scn.array));
        }
        else
        {
            AGG(rc, writer.named(*scn.array, "array"));
        }
    }
    if (scn.text)
    {
        if (scn.as_typed)
        {
            AGG(rc, writer.typed(*scn.text));
        }
        else
        {
            AGG(rc, writer.named(*scn.text, "text"));
        }
    }
    REQUIRE(exp.rc == rc);
    std::cout << "***" << std::endl;
    std::cout << str.str() << std::endl;
    std::cout << "***" << std::endl;
    REQUIRE(exp.output == str.str());
}
