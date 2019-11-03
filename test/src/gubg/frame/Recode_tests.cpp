#include <gubg/frame/Encoder.hpp>
#include <gubg/frame/Decoder.hpp>
#include <gubg/Range_macro.hpp>
#include <catch.hpp>
#include <optional>
#include <iostream>
#include <iomanip>
using namespace gubg::frame;

namespace  { 
    using Payload = std::vector<std::uint8_t>;

    class Decoder: public Decoder_crtp<Decoder>
    {
    public:
        std::optional<std::uint64_t> type;
        std::optional<Payload> payload;

        void on_soh()
        {
            std::cout << "Found SOH\n";
        }
        void on_type(std::uint64_t type)
        {
            std::cout << "Found type " << type << "\n";
        }
        void on_size(std::uint64_t size)
        {
            std::cout << "Found size " << size << "\n";
        }
        void on_payload(std::uint64_t type, Payload &payload)
        {
            std::string str(payload.size(), '?');
            std::copy(RANGE(payload), str.begin());
            std::cout << "Found payload of type " << std::hex << type << std::dec << ": " << str.size() << " " << str << "\n";
            this->type = type;
            this->payload = payload;
        }
        void error_broken_frame()
        {
            std::cout << "Error: broken frame\n";
        }
        void error_expected_soh(std::uint8_t byte)
        {
            std::cout << "Error: expected soh iso " << (int)byte << "\n";
        }
        void error_missing_msb(std::uint8_t byte)
        {
            std::cout << "Error: missing msb in " << (int)byte << "\n";
        }

    private:
    };
} 

TEST_CASE("frame::Encoder and frame::Decoder tests", "[ut][frame]")
{
    std::uint8_t soh{};
    std::uint64_t type{};
    std::string str;

    SECTION("soh == 0xa5")
    {
        soh = 'a';
        SECTION("type == 1")
        {
            type = 0x1234567890;
            str = std::string(1234, 'b');
        }
    }

    Payload payload;
    payload.resize(str.size());
    std::copy(RANGE(str), payload.begin());
    const Payload orig_payload = payload;

    Encoder encoder{soh};
    REQUIRE(encoder.encode(type, payload));

    Decoder decoder{soh};
    for (auto byte: encoder.header)
    {
        std::cout << "header: " << std::hex << (int)byte << std::dec << std::endl;
        decoder.process(byte);
    }
    for (auto byte: payload)
        decoder.process(byte);

    REQUIRE(!!decoder.type);
    REQUIRE(decoder.type.value() == type);
    REQUIRE(decoder.payload.value() == orig_payload);
    std::cout << "Header size: " << encoder.header.size() << "\n";
}

