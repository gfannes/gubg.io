#include "catch.hpp"
#include "gubg/s11n/Reader.hpp"
#include "gubg/debug.hpp"
#include <optional>
#include <string>
using namespace gubg;

TEST_CASE("s11n::Reader tests", "[ut][s11n][Reader]")
{
    S("");

    struct Scn
    {
        std::string message;
    };
    struct Exp
    {
        std::optional<std::vector<int>> array;
        std::optional<std::string> text;
    };
    Scn scn;
    Exp exp;

    SECTION("array")
    {
        scn.message = "[array](size:3){[0](value:0)[1](value:1)[2](value:2)}";
        exp.array.emplace();
        exp.array->push_back(0);
        exp.array->push_back(1);
        exp.array->push_back(2);
    }
    SECTION("text")
    {
        scn.message = "[text](encoding:as_is){abc}";
        exp.text.emplace();
        *exp.text = "abc";
    }
    
    L(C(scn.message));
    s11n::Reader reader(scn.message);

    {
        std::optional<std::vector<int>> array;
        const auto ok = reader.named("array", [&](auto &rdr){
                MSS_BEGIN(bool);

                unsigned int size;
                MSS(rdr.attr(size, "size"));

                array.emplace();
                array->resize(size);

                for (auto ix = 0u; ix < size; ++ix)
                    MSS(rdr.named(ix, [&](auto &rdr) { return rdr.attr((*array)[ix], "value"); }));

                MSS_END();
                });
        REQUIRE(ok == !!exp.array);
        if (ok)
        {
            REQUIRE(!!array);
            REQUIRE(*array == *exp.array);
        }
    }
    {
        std::optional<std::string> text;
        const auto ok = reader.named("text", [&](auto &rdr){
                text.emplace();
                *text = rdr.text();
                return true;
                });
        REQUIRE(ok == !!exp.text);
        if (ok)
        {
            REQUIRE(!!text);
            REQUIRE(*text == *exp.text);
        }
    }
}
