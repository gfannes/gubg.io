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
        SECTION("root via doc")
        {
            auto root = doc.node("root");
            SECTION("no attr")
            {
                wanted = "[root]\n";
            }
            SECTION("with attr")
            {
                wanted = "[root](a:b)\n";
                root.attr("a", "b");
            }
        }
        SECTION("root via node")
        {
            wanted = "[root]\n";
            naft::Node &node = doc;
            auto root = node.node("root");
        }
    }
    REQUIRE(oss.str() == wanted);
}
