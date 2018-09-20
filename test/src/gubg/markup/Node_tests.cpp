#include "catch.hpp"
#include "gubg/markup/Node.hpp"
#include "gubg/markup/Markdown.hpp"
#include <sstream>
#include <iostream>
using namespace gubg::markup;

TEST_CASE("markup::Node tests", "[ut][markup][Node]")
{
    auto doc = Node<Markdown>(Document{"Title"});
    {
        auto &sec = doc.add(Section{"Section 1"});
        {
            auto &subsec = sec.add(Section{"Subsection 1.1"});
        }
    }
    {
        auto &sec = doc.add(Section{"Section 2"});
    }

    std::ostringstream oss;
    Markdown md;

    REQUIRE(doc.render(oss, md));

    std::cout << oss.str() << std::endl;
}
