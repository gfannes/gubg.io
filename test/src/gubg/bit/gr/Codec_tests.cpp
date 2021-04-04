#include <gubg/bit/gr/Codec.hpp>
#include <gubg/hr.hpp>
#include <catch.hpp>
using namespace gubg::bit;

TEST_CASE("bit::gr::Codec unsigned tests", "[ut][bit][gr][Codec][unsigned]")
{
    using T = unsigned int;

    struct Scn
    {
        gr::Metadata md;
        T v = 0;
    };

    Scn scn;

    SECTION("md 0")
    {
        scn.md.remainder_bw = 0;
        SECTION("v 0") { scn.v = 0; }
        SECTION("v 1") { scn.v = 1; }
        SECTION("v 2") { scn.v = 2; }
        SECTION("v 32") { scn.v = 32; }
    }
    SECTION("md 2")
    {
        scn.md.remainder_bw = 2;
        SECTION("v 0") { scn.v = 0; }
        SECTION("v 1") { scn.v = 1; }
        SECTION("v 2") { scn.v = 2; }
        SECTION("v 32") { scn.v = 32; }
    }

    gr::Codec<T> cdc;

    Writer writer;

    cdc.encode(writer, scn.md, scn.v);

    std::vector<int> bits;
    writer.to_bits(bits);
    std::cout << gubg::hr(bits) << std::endl;

    std::vector<std::uint8_t> bytes;
    writer.to_bytes(bytes);

    Reader reader{bytes.data(), bytes.size()};

    T v = ~0;

    cdc.decode(v, scn.md, reader);

    REQUIRE(v == scn.v);
}

TEST_CASE("bit::gr::Codec signed tests", "[ut][bit][gr][Codec][signed]")
{
    using T = int;

    struct Scn
    {
        gr::Metadata md;
        T v = 0;
    };

    Scn scn;

    SECTION("md 0")
    {
        scn.md.remainder_bw = 0;
        SECTION("v 0") { scn.v = 0; }
        SECTION("v 1") { scn.v = 1; }
        SECTION("v 2") { scn.v = 2; }
        SECTION("v 32") { scn.v = 32; }
        SECTION("v -1") { scn.v = -1; }
        SECTION("v -2") { scn.v = -2; }
        SECTION("v -32") { scn.v = -32; }
    }
    SECTION("md 2")
    {
        scn.md.remainder_bw = 2;
        SECTION("v 0") { scn.v = 0; }
        SECTION("v 1") { scn.v = 1; }
        SECTION("v 2") { scn.v = 2; }
        SECTION("v 32") { scn.v = 32; }
        SECTION("v -1") { scn.v = -1; }
        SECTION("v -2") { scn.v = -2; }
        SECTION("v -32") { scn.v = -32; }
    }

    gr::Codec<T> cdc;

    Writer writer;

    cdc.encode(writer, scn.md, scn.v);

    std::vector<int> bits;
    writer.to_bits(bits);
    std::cout << gubg::hr(bits) << std::endl;

    std::vector<std::uint8_t> bytes;
    writer.to_bytes(bytes);

    Reader reader{bytes.data(), bytes.size()};

    T v = ~0;

    cdc.decode(v, scn.md, reader);

    REQUIRE(v == scn.v);
}
