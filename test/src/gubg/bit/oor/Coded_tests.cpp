#include <gubg/bit/oor/Codec.hpp>
#include <catch.hpp>
#include <vector>
using namespace gubg::bit;

TEST_CASE("bit::oor::Codec tests", "[ut][bit][oor][Codec]")
{
    using T = unsigned int;
    using CDC = oor::Codec<T>;

    CDC cdc;

    std::vector<T> data = {0,0,0,1,1,15};

    cdc.encode(data.data(), data.size());
}
