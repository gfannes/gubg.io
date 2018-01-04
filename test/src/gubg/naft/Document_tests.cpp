#include "catch.hpp"
#include "gubg/naft/Document.hpp"
#include <sstream>
using namespace gubg;

TEST_CASE("naft::Document tests", "[ut][naft]")
{
    std::ostringstream oss;
    std::string wanted;
    {
        naft::Document doc(oss);
        SECTION("root")
        {
            wanted = "[root]\n\n";
            auto root = doc.node("root");
        }
    }
    REQUIRE(oss.str() == wanted);
}
