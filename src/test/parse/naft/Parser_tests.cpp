#include "catch.hpp"
#include "gubg/debug.hpp"
#include "gubg/parse/naft/Parser.hpp"
#include <string>
#include <vector>
#include <cassert>

namespace  { 
    class Parser: public gubg::parse::naft::Parser_crtp<Parser>
    {
    public:
        std::vector<std::string> result;

        bool naft_text(const std::string &text)
        {
            result.push_back("text:"+text);
            return true;
        }
        bool naft_node_open(const std::string &tag)
        {
            result.push_back("open:"+tag);
            return true;
        }
        bool naft_attr(const std::string &key, const std::string &value)
        {
            result.push_back("attr:"+key+"=>"+value);
            return true;
        }
        bool naft_attr_done()
        {
            result.push_back("attr_done");
            return true;
        }
        bool naft_node_close()
        {
            result.push_back("close");
            return true;
        }
    private:
    };
} 

TEST_CASE("naft::Parser tests", "[ut][naft]")
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
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("two tags")
    {
        content = "[tag_a][tag_b]";
        wanted.push_back("open:tag_a");
        wanted.push_back("attr_done");
        wanted.push_back("close");
        wanted.push_back("open:tag_b");
        wanted.push_back("attr_done");
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
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("2 x (tag + attr)")
    {
        content = "[tag0](a:0)[tag1](b:1)";
        wanted.push_back("open:tag0");
        wanted.push_back("attr:a=>0");
        wanted.push_back("attr_done");
        wanted.push_back("close");
        wanted.push_back("open:tag1");
        wanted.push_back("attr:b=>1");
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("nested")
    {
        content = "[tag0]{[tag1]}}";
        wanted.push_back("open:tag0");
        wanted.push_back("attr_done");
        wanted.push_back("open:tag1");
        wanted.push_back("attr_done");
        wanted.push_back("close");
        wanted.push_back("close");
        wanted.push_back("text:}");
    }
    SECTION("tag with []")
    {
        content = "[tag[]]";
        wanted.push_back("open:tag[]");
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("attr with ()")
    {
        content = "[tag](a():0)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a()=>0");
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("key with :")
    {
        content = "[tag](a:b:c)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a=>b:c");
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("comment")
    {
        content = "[tag](a:0)```(b:1)```(c:2)";
        wanted.push_back("open:tag");
        wanted.push_back("attr:a=>0");
        wanted.push_back("text:```(b:1)```");
        wanted.push_back("attr:c=>2");
        wanted.push_back("attr_done");
        wanted.push_back("close");
    }
    SECTION("{ before a tag")
    {
        content = "{}";
        wanted.push_back("text:{}");
    }

    Parser parser;
    REQUIRE(parser.process(content));

    REQUIRE(parser.result == wanted);
}
