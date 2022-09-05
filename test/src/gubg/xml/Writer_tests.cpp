#include <gubg/xml/Writer.hpp>

#include <catch.hpp>

#include <iostream>
#include <sstream>

using namespace gubg;

TEST_CASE("xml::Writer tests", "[ut][xml][Writer]")
{
    struct Scn
    {
        std::function<void(xml::Writer &)> lambda;
    };
    struct Exp
    {
        std::string xml;
    };

    Scn scn;
    Exp exp;

    SECTION("Prolog")
    {
        scn.lambda = [](auto &w){
            w.prolog("version", "1.0", "encoding", "UTF-8");
        };
        exp.xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    }

    std::ostringstream oss;
    xml::Writer writer{oss};
    scn.lambda(writer);
    REQUIRE(oss.str() == exp.xml);
}
