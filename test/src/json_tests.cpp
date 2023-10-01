#include <nlohmann/json.hpp>

#include <catch.hpp>

#include <iostream>

TEST_CASE("nlohmann/json test", "[ut][nlohmann][json]")
{
    struct Scn
    {
        nlohmann::json obj;
    };
    Scn scn;

    struct Exp
    {
        std::string dump_str;
    };
    Exp exp;

    SECTION("key-value")
    {
        scn.obj = {
            {"key", "value"},
        };
        exp.dump_str = R"({"key":"value"})";
    }

    REQUIRE(scn.obj.dump() == exp.dump_str);
}
