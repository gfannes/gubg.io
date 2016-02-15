#include "catch.hpp"
#include "gubg/parse/xml/Coding.hpp"
using namespace gubg;
using namespace gubg::xml;
using namespace std;

namespace 
{
    bool checkEncodingOK(const string &raw, const string &wanted)
    {
        MSS_BEGIN(bool);
        string enc;
        MSS(encode(enc, raw));
        MSS(wanted == enc);
        MSS_END();
    }
    bool checkEncodingKO(const string &raw)
    {
        string enc;
        return !mss::is_ok(encode(enc, raw));
    }
    bool checkDecodingOK(const string &enc, const string &wanted)
    {
        MSS_BEGIN(bool);
        string raw;
        MSS(decode(raw, enc));
        MSS(wanted == raw);
        MSS_END();
    }
    bool checkDecodingKO(const string &enc)
    {
        string raw;
        return !mss::is_ok(decode(raw, enc));
    }
}

TEST_CASE("xml encoding tests", "[ut][xml]")
{
    SECTION("pos")
    {
        REQUIRE(checkEncodingOK("", ""));
        REQUIRE(checkEncodingOK(" ", " "));
        REQUIRE(checkEncodingOK("\n", "\n"));
        REQUIRE(checkEncodingOK("\r", "\r"));
        REQUIRE(checkEncodingOK("\t", "\t"));
        REQUIRE(checkEncodingOK("abc", "abc"));
        REQUIRE(checkEncodingOK("\"", "&quot;"));
        REQUIRE(checkEncodingOK("<", "&lt;"));
        REQUIRE(checkEncodingOK(">", "&gt;"));
        REQUIRE(checkEncodingOK("&", "&amp;"));
    }
    SECTION("neg")
    {
        REQUIRE(checkEncodingKO("\x01"));
    }
}
TEST_CASE("xml decoding tests", "[ut][xml]")
{
    SECTION("pos")
    {
        REQUIRE(checkDecodingOK("", ""));
        REQUIRE(checkDecodingOK(" ", " "));
        REQUIRE(checkDecodingOK("\n", "\n"));
        REQUIRE(checkDecodingOK("\r", "\r"));
        REQUIRE(checkDecodingOK("\t", "\t"));
        REQUIRE(checkDecodingOK("abc", "abc"));
        REQUIRE(checkDecodingOK("&quot;", "\""));
        REQUIRE(checkDecodingOK("&lt;", "<"));
        REQUIRE(checkDecodingOK("&gt;", ">"));
        REQUIRE(checkDecodingOK("&amp;", "&"));
        REQUIRE(checkDecodingOK("&amp;&amp;", "&&"));
    }
    SECTION("neg")
    {
        REQUIRE(checkDecodingKO("<"));
        REQUIRE(checkDecodingKO(">"));
        REQUIRE(checkDecodingKO("\""));
        REQUIRE(checkDecodingKO("&"));
        REQUIRE(checkDecodingKO("&quot"));
        REQUIRE(checkDecodingKO("&abc"));
    }
}
