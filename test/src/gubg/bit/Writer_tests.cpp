#include <gubg/bit/Writer.hpp>
#include <gubg/hr.hpp>
#include <catch.hpp>
#include <iostream>
using namespace gubg;

TEST_CASE("bit::Writer tests", "[ut][bit][Writer]")
{
    struct Exp
    {
        std::vector<std::uint8_t> bytes;
    };
    Exp exp;

    bit::Writer w;

    SECTION("fresh")
    {
        REQUIRE(w.bit_size() == 0);
        REQUIRE(w.byte_size() == 0);
    }
    SECTION("1")
    {
        w.one();
        REQUIRE(w.bit_size() == 1);
        REQUIRE(w.byte_size() == 1);
        exp.bytes.emplace_back(0x01);
    }
    SECTION("0")
    {
        w.zero();
        exp.bytes.emplace_back(0x00);
    }
    SECTION("1010 0001 0xa1")
    {
        SECTION("via zero()/one()")
        {
            w.one();
            w.zero();
            w.zero();
            w.zero();

            w.zero();
            w.one();
            w.zero();
            w.one();
        }
        SECTION("via uint()")
        {
            w.uint(0xa1, 8);
        }
        REQUIRE(w.bit_size() == 8);
        REQUIRE(w.byte_size() == 1);
        exp.bytes.emplace_back(0xa1);
    }
    SECTION("1 1010 0001 0x01a1")
    {
        SECTION("via zero()/one()")
        {
            w.one();
            w.zero();
            w.zero();
            w.zero();

            w.zero();
            w.one();
            w.zero();
            w.one();

            w.one();
        }
        SECTION("via uint()")
        {
            w.uint(0x01a1, 9);
        }
        REQUIRE(w.bit_size() == 9);
        REQUIRE(w.byte_size() == 2);
        exp.bytes.emplace_back(0xa1);
        exp.bytes.emplace_back(0x01);
    }

    std::vector<std::uint8_t> bytes;
    w.to_bytes(bytes);
    if (bytes != exp.bytes)
    {
        std::cout << "actual: " << gubg::hr_hex(bytes) << std::endl;
        std::cout << "expect: " << gubg::hr_hex(exp.bytes) << std::endl;
        REQUIRE(false);
    }
}
