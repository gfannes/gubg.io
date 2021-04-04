#include <gubg/bit/gr/Codec.hpp>
#include <gubg/hr.hpp>
#include <gubg/mss.hpp>
#include <catch.hpp>
using namespace gubg::bit;

namespace  { 
    template <typename T, gr::Type GrType>
    bool test()
    {
        MSS_BEGIN(bool);

        gr::Codec<T, GrType> cdc;
        gr::Metadata md;

        for (unsigned int remainder_bw: {0u, 2u})
        {
            for (int v: {0, 1, 2, 32, -1, -2, -32})
            {
                const bool is_signed = std::is_signed_v<T>;
                if (!is_signed && v < 0)
                    break;

                std::cout << "[GR]" << C(is_signed)C(GrType, int)C(remainder_bw)C(v);

                md.remainder_bw = remainder_bw;

                Writer writer;

                cdc.encode(writer, md, v);

                std::vector<int> bits;
                writer.to_bits(bits);
                std::cout << gubg::hr(bits) << std::endl;

                std::vector<std::uint8_t> bytes;
                writer.to_bytes(bytes);

                Reader reader{bytes.data(), bytes.size()};

                T vv = ~0;

                cdc.decode(vv, md, reader);

                MSS(vv == v);
            }
        }

        MSS_END();
    }
} 

TEST_CASE("bit::gr::Codec tests", "[ut][bit][gr][Codec]")
{
    REQUIRE(test<unsigned int, gr::Type::Normal>());
    REQUIRE(test<         int, gr::Type::Normal>());
    REQUIRE(test<unsigned int, gr::Type::Exponential>());
    REQUIRE(test<         int, gr::Type::Exponential>());
}
