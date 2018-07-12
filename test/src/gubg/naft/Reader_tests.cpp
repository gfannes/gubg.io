#include "catch.hpp"
#include "gubg/naft/Reader.hpp"

TEST_CASE("naft::Reader tests", "[ut][naft][Reader]")
{
    S("");

    struct Scn
    {
        std::string str;
    };
    struct Exp
    {
        bool name_ok = false;
        bool type_ok = false;
    };

    Scn scn;
    Exp exp;

    SECTION("empty")
    {
    }
    SECTION("name")
    {
        scn.str = "[name](a:42.1)(b){}";
        exp.name_ok = true;
    }
    SECTION("type")
    {
        scn.str = "[:type](a:42.1)(b){}";
        exp.type_ok = true;
    }

    gubg::naft::Reader reader{scn.str};

    bool ok = false;
    if (!ok)
    {
        ok = reader.pop_name("name");
        REQUIRE(ok == exp.name_ok);
    }
    if (!ok)
    {
        ok = reader.pop_type("type");
        REQUIRE(ok == exp.type_ok);
    }
    if (ok)
    {
        REQUIRE(reader.has_attr("b"));
        {
            std::string v;
            REQUIRE(reader.get_attr("a", v));
            REQUIRE(v == "42.1");
        }
        {
            int v;
            REQUIRE(reader.get_attr("a", v));
            REQUIRE(v == 42);
        }
        {
            float v;
            REQUIRE(reader.get_attr("a", v));
            REQUIRE(v == Approx(42.1));
        }
    }
}
