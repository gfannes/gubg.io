#include <gubg/naft/Reader.hpp>

#include <catch.hpp>

#include <optional>
#include <sstream>

using namespace gubg;

TEST_CASE("naft::Reader tests", "[ut][naft][Reader]")
{
    S("");

    struct Scn
    {
        std::string str;
    };
    struct Exp
    {
        std::ostringstream repr;
        std::optional<std::string> error;
    };

    Scn scn;
    Exp exp;

    SECTION("empty") { }
    SECTION("whitespace") { scn.str = " \t\n\r\t \n\r"; }
    SECTION("[a]")
    {
        scn.str = "[a]";
        exp.repr << " >>a <<a";
    }
    SECTION("[a](b)(c)")
    {
        scn.str = "[a](b)(c)";
        exp.repr << " >>a @b @c <<a";
    }
    SECTION("[a]t")
    {
        scn.str = "[a]t";
        exp.repr << " >>a <<a t";
    }
    SECTION("[a]{t}")
    {
        scn.str = "[a]{t}";
        exp.repr << " >>a t <<a";
    }
    SECTION("[a]{[b][c]}")
    {
        scn.str = "[a]{[b][c]}";
        exp.repr << " >>a >>b <<b >>c <<c <<a";
    }

    naft::Reader reader{scn.str};

    std::ostringstream repr;
    for (naft::Reader::Item item; reader(item); )
    {
        L(C(item));
        using What = naft::Reader::Item::What;
        switch (item.what)
        {
            case What::NodeOpen: repr << " >>" << item.text; break;
            case What::NodeClose: repr << " <<" << item.text; break;
            case What::Attribute: repr << " @" << item.text; break;
            case What::Text: repr << " " << item.text; break;
        }
    }

    if (reader.error)
        std::cout << *reader.error << std::endl;
    REQUIRE(reader.error == exp.error);

    REQUIRE(repr.str() == exp.repr.str());
}
