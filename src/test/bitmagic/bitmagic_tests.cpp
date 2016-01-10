#include "catch.hpp"
#include "gubg/bitmagic/bitmagic.hpp"
using namespace gubg::bitmagic;

TEST_CASE("bitmagic tests", "[ut]")
{
}
int main()
{
    SECTION("rotateLeft")
    {
        REQUIRE(0x12345678 == rotateLeft(0x12345678, 0));
        REQUIRE(0x2468acf0 == rotateLeft(0x12345678, 1));
        REQUIRE(0x23456781 == rotateLeft(0x12345678, 4));
        REQUIRE(0x34567812 == rotateLeft(0x12345678, 8));
        REQUIRE(0x45678123 == rotateLeft(0x12345678, 12));
        REQUIRE(0x56781234 == rotateLeft(0x12345678, 16));
        REQUIRE(0x67812345 == rotateLeft(0x12345678, 20));
        REQUIRE(0x78123456 == rotateLeft(0x12345678, 24));
        REQUIRE(0x81234567 == rotateLeft(0x12345678, 28));
        REQUIRE(0x12345678 == rotateLeft(0x12345678, 32));
    }
    SECTION("rotateRight")
    {
        REQUIRE(0x12345678 == rotateRight(0x12345678, 0));
        REQUIRE(0x091a2b3c == rotateRight(0x12345678, 1));
        REQUIRE(0x81234567 == rotateRight(0x12345678, 4));
        REQUIRE(0x78123456 == rotateRight(0x12345678, 8));
        REQUIRE(0x67812345 == rotateRight(0x12345678, 12));
        REQUIRE(0x56781234 == rotateRight(0x12345678, 16));
        REQUIRE(0x45678123 == rotateRight(0x12345678, 20));
        REQUIRE(0x34567812 == rotateRight(0x12345678, 24));
        REQUIRE(0x23456781 == rotateRight(0x12345678, 28));
        REQUIRE(0x12345678 == rotateRight(0x12345678, 32));
    }
}
