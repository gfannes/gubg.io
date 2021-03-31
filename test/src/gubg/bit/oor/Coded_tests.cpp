#include <gubg/bit/oor/Codec.hpp>
#include <gubg/hr.hpp>
#include <gubg/Range_macro.hpp>
#include <catch.hpp>
#include <vector>
#include <iostream>
using namespace gubg::bit;

TEST_CASE("bit::oor::Codec uint tests", "[ut][bit][oor][Codec][uint]")
{
    using Data_u = std::vector<unsigned int>;
    using Data_i = std::vector<int>;
    using Bits = std::vector<int>;
    using Bytes = std::vector<std::uint8_t>;

    struct Scn
    {
        Data_u data_u;
        Data_i data_i;
    };

    struct Exp
    {
        oor::Metadata md;
        Bits bits;
    };

    Scn scn;
    Exp exp;

    SECTION("0,0,0,0")
    {
        scn.data_u = {0,0,0,0};
        exp.md.max_bw = 0;
        exp.md.min_bw = 0;
        exp.bits = {};
    }
    SECTION("0,0,0,1")
    {
        scn.data_u = {0,0,0,1};
        exp.md.max_bw = 1;
        exp.md.min_bw = 0;
        exp.bits = {0, 0, 0, 1};
    }
    SECTION("0,0,0,7")
    {
        scn.data_u = {0,0,0,7};
        exp.md.max_bw = 3;
        exp.md.min_bw = 1;
        exp.bits = {0, 0, 0, 1,1,1,1};
    }
    SECTION("6,6,6,255")
    {
        scn.data_u = {6,6,6,255};
        exp.md.max_bw = 8;
        exp.md.min_bw = 3;
        exp.bits = {0,1,1, 0,1,1, 0,1,1, 1,1,1,1,1,1,1,1,1,1,1};
    }
    SECTION("-16,-15,-1,0,1,15")
    {
        scn.data_i = {-16,-15,-1,0,1,15};
    }

    {
        std::cout << "unsigned int" << std::endl;
        using CDC_u = oor::Codec<unsigned int>;
        CDC_u cdc_u;

        oor::Metadata md;
        cdc_u.find_optimal_metadata(md, scn.data_u.data(), scn.data_u.size());
        std::cout << "Optimal md: " << " " << md.max_bw << " " << md.min_bw << std::endl;

        Writer writer;
        cdc_u.encode(writer, md, scn.data_u.data(), scn.data_u.size());

        Bits bits;
        writer.to_bits(bits);
        std::cout << gubg::hr(bits) << std::endl;

        REQUIRE(md.max_bw == exp.md.max_bw);
        REQUIRE(md.min_bw == exp.md.min_bw);

        REQUIRE(bits == exp.bits);

        Bytes bytes;
        writer.to_bytes(bytes);

        Reader reader{bytes.data(), bytes.size()};

        Data_u data_u;

        //Somehow, we have to know the size
        data_u.resize(scn.data_u.size());

        cdc_u.decode(data_u.data(), data_u.size(), md, reader);

        REQUIRE(data_u == scn.data_u);
    }

    {
        std::cout << "int" << std::endl;
        if (scn.data_i.empty() && !scn.data_u.empty())
        {
            scn.data_i.resize(scn.data_u.size());
            std::copy(RANGE(scn.data_u), scn.data_i.begin());
        }

        using CDC_i = oor::Codec<int>;
        CDC_i cdc_i;

        oor::Metadata md;
        cdc_i.find_optimal_metadata(md, scn.data_i.data(), scn.data_i.size());
        std::cout << "Optimal md: " << " " << md.max_bw << " " << md.min_bw << std::endl;

        Writer writer;
        cdc_i.encode(writer, md, scn.data_i.data(), scn.data_i.size());

        Bits bits;
        writer.to_bits(bits);
        std::cout << gubg::hr(bits) << std::endl;

        Bytes bytes;
        writer.to_bytes(bytes);

        Reader reader{bytes.data(), bytes.size()};

        Data_i data_i;

        //Somehow, we have to know the size
        data_i.resize(scn.data_i.size());

        cdc_i.decode(data_i.data(), data_i.size(), md, reader);

        REQUIRE(data_i == scn.data_i);
    }
}
