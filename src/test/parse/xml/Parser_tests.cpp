#include "catch.hpp"
#include "gubg/parse/xml/Parser.hpp"
#include "gubg/file/Filesystem.hpp"
using gubg::xml::ReturnCode;

namespace data
{
    const char * open_close = "<abc></abc>";
    const char * openclose = "<abc/>";
    const char * open_close_attr = "<abc id=\"1\"></abc>";
    const char * openclose_attr = "<abc id=\"1\"/>";
    const char * text = "<abc>text</abc>";
    const char * comment = "<abc><!--this is a comment--></abc>";
    const char * cdata = "<abc><![CDATA[this is cdata <a><b><c>]]></abc>";
    namespace illegal
    {
        const char * duplicate_attr = "<abc id=\"1\" id=\"2\"></abc>";
    }
}

namespace 
{
    const char *logns = "test";

    struct Parser: gubg::xml::Parser_crtp<Parser>
    {
        typedef gubg::xml::Path Path;
        typedef gubg::xml::Attributes Attributes;
        void parser_text(const std::string &text, const Path &path)
        {
            S(logns);L(text);
        }
        void parser_open(const std::string &tag, const Path &path)
        {
            S(logns);L(tag);
        }
        void parser_close(const std::string &tag, const Path &path)
        {
            S(logns);L(tag);
        }
        void parser_attr(const Attributes &attrs, const Path &path)
        {
            S(logns);L(attrs.size());
            for (auto attr: attrs)
            {
                L(STREAM(attr.first, attr.second));
            }
        }
        void parser_comment(const std::string &comment, const Path &path)
        {
            S(logns);L(comment);
        }
    };
}

TEST_CASE("xml parser tests", "[ut][xml]")
{
    Parser p;
    SECTION("pos")
    {
        REQUIRE(ReturnCode::OK == p.process(data::open_close));
        REQUIRE(ReturnCode::OK == p.process(data::openclose));
        REQUIRE(ReturnCode::OK == p.process(data::open_close_attr));
        REQUIRE(ReturnCode::OK == p.process(data::openclose_attr));
        REQUIRE(ReturnCode::OK == p.process(data::text));
        REQUIRE(ReturnCode::OK == p.process(data::comment));
        REQUIRE(ReturnCode::OK == p.process(data::cdata));
    }
    SECTION("neg")
    {
        REQUIRE(ReturnCode::OK != p.process(data::illegal::duplicate_attr));
    }
    if (false)
    {
        SECTION("freeplane")
        {
            std::string str;
            gubg::file::read(str, gubg::file::File("/home/gfannes/cryptx-avalon/Overview.mm"));
            REQUIRE(ReturnCode::OK == p.process(str));
        }
    }
}
