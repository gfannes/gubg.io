#include <gubg/bit/oor/util.hpp>
#include <catch.hpp>
using namespace gubg::bit::oor;

TEST_CASE("bit::oor::util tests", "[ut][bit][oor][util]")
{
    REQUIRE(util::Max<std::uint8_t> == 255u);

    REQUIRE(util::bitwidth<std::uint8_t>(0) == 1);
    REQUIRE(util::bitwidth<std::uint8_t>(1) == 2);
    REQUIRE(util::bitwidth<std::uint8_t>(2) == 2);
    REQUIRE(util::bitwidth<std::uint8_t>(3) == 3);
    REQUIRE(util::bitwidth<std::uint8_t>(4) == 3);
    REQUIRE(util::bitwidth<std::uint8_t>(5) == 3);
    REQUIRE(util::bitwidth<std::uint8_t>(6) == 3);
    REQUIRE(util::bitwidth<std::uint8_t>(7) == 4);

    REQUIRE(util::bitwidth<std::uint8_t>(126) == 7);
    REQUIRE(util::bitwidth<std::uint8_t>(127) == 8);
    REQUIRE(util::bitwidth<std::uint8_t>(128) == 8);

    REQUIRE(util::bitwidth<std::uint8_t>(254) == 8);
    REQUIRE(util::bitwidth<std::uint8_t>(255) == 8);
}
