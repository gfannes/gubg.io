#include <gubg/bits/Writer.hpp>
#include <catch.hpp>
using namespace gubg;

TEST_CASE("bits::Writer tests", "[ut][bits][Writer]")
{
    struct Exp
    {
        std::vector<std::uint8_t> bytes;
    };
    Exp exp;

    bits::Writer w;

    SECTION("fresh") { }
    SECTION("1")
    {
        w.one();
        exp.bytes.emplace_back(0x01);
    }
    SECTION("0")
    {
        w.zero();
        exp.bytes.emplace_back(0x00);
    }
    SECTION("0xa1")
    {
        SECTION("via zero()/one()")
        {
            //a => 5
            w.one();
            w.zero();
            w.one();
            w.zero();

            //1 => 8
            w.zero();
            w.zero();
            w.zero();
            w.one();
        }
        SECTION("via uint()")
        {
            w.uint<8>(0xa1);
        }
        exp.bytes.emplace_back(0x85);
    }

    std::vector<std::uint8_t> bytes;
    w.to_bytes(bytes);
    REQUIRE(bytes == exp.bytes);
}
