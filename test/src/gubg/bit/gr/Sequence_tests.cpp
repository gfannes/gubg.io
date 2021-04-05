#include <gubg/bit/gr/Sequence.hpp>
#include <catch.hpp>
#include <vector>
#include <iostream>
using namespace gubg::bit;

TEMPLATE_TEST_CASE("bit::gr::Sequence tests", "[ut][bit][gr][Sequence]", unsigned int, int)
{
    struct Scn
    {
        std::vector<TestType> data;
    };
    Scn scn;

    SECTION("no data")
    {
    }
    SECTION("unsigned data")
    {
        scn.data = {0u, 1u, 2u, 3u, 1024u*1024u, 3u, 2u, 1u, 0u};
    }
    if constexpr (std::is_same_v<TestType, int>)
    {
        SECTION("signed data")
        {
            scn.data = {0, -1, 2, -3, 1024*1024, 3, -2, 1, 0};
        }
    }

    for (auto how = 0u; how < 4; ++how)
    {
        const bool encode_one = !(how&0b01);
        const bool decode_one = !(how&0b10);

        gr::Sequence<gr::Type::Exponential> s;
        Writer writer;

        if (encode_one)
        {
            for (auto v: scn.data)
                s.encode(writer, v);
        }
        else
        {
            s.encode(writer, scn.data.data(), scn.data.size());
        }

        std::cout << C(s.count())C(writer.bit_size()) << std::endl;

        std::vector<std::uint8_t> bytes;
        writer.to_bytes(bytes);
        Reader reader{bytes.data(), bytes.size()};

        const auto size = s.count();
        REQUIRE(s.count() == scn.data.size());

        if (decode_one)
        {
            for (const auto ref_v: scn.data)
            {
                TestType v = ~0;
                s.decode(v, reader);
                REQUIRE(v == ref_v);
            }
        }
        else
        {
            std::vector<TestType> data;
            data.resize(s.count());
            s.decode(data.data(), data.size(), reader);
            REQUIRE(data == scn.data);
        }
    }
}
