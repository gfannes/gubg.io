#include <gubg/bit/oor/Codec.hpp>
#include <gubg/hr.hpp>
#include <catch.hpp>
#include <vector>
#include <iostream>
using namespace gubg::bit;

TEST_CASE("bit::oor::Codec tests", "[ut][bit][oor][Codec]")
{
    using T = unsigned int;
    using CDC = oor::Codec<T>;

    using Data = std::vector<T>;
    using Bits = std::vector<int>;
    using Bytes = std::vector<std::uint8_t>;

    struct Scn
    {
        Data data;
    };

    struct Exp
    {
        CDC::Metadata md;
        Bits bits;
    };

    Scn scn;
    Exp exp;

    SECTION("0,0,0,0")
    {
        scn.data = {0,0,0,0};
        exp.md.max_bw = 0;
        exp.md.min_bw = 0;
        exp.bits = {};
    }
    SECTION("0,0,0,1")
    {
        scn.data = {0,0,0,1};
        exp.md.max_bw = 1;
        exp.md.min_bw = 0;
        exp.bits = {0, 0, 0, 1};
    }
    SECTION("0,0,0,7")
    {
        scn.data = {0,0,0,7};
        exp.md.max_bw = 3;
        exp.md.min_bw = 1;
        exp.bits = {0, 0, 0, 1,1,1,1};
    }
    SECTION("6,6,6,255")
    {
        scn.data = {6,6,6,255};
        exp.md.max_bw = 8;
        exp.md.min_bw = 3;
        exp.bits = {0,1,1, 0,1,1, 0,1,1, 1,1,1,1,1,1,1,1,1,1,1};
    }

    CDC cdc;
    CDC::Metadata md;

    Writer writer;

    cdc.encode(md, writer, scn.data.data(), scn.data.size());
    std::cout << "Optimal md: " << " " << md.max_bw << " " << md.min_bw << std::endl;

    Bits bits;
    writer.to_bits(bits);
    std::cout << gubg::hr(bits) << std::endl;

    REQUIRE(md.max_bw == exp.md.max_bw);
    REQUIRE(md.min_bw == exp.md.min_bw);

    REQUIRE(bits == exp.bits);

    Bytes bytes;
    writer.to_bytes(bytes);

    Reader reader{bytes.data(), bytes.size()};

    Data data;

    //Somehow, we have to know the size
    data.resize(scn.data.size());

    cdc.decode(data.data(), data.size(), md, reader);

    REQUIRE(data == scn.data);
}
