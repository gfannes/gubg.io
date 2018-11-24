#include "catch.hpp"
#include "gubg/serial/Endpoint.hpp"
#include "gubg/macro/capture.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
using namespace gubg;

TEST_CASE("serial::Endpoint tests", "[manual][serial][Endpoint]")
{
    serial::Endpoint ep0;

    REQUIRE(!ep0.valid());
    REQUIRE(!ep0.open("DoesNotExist"));
    REQUIRE(!ep0.valid());
    REQUIRE(ep0.open("/dev/ttyUSB0"));
    REQUIRE(ep0.valid());

    serial::Settings settings;
    REQUIRE(ep0.get(settings));
    std::cout << C(settings.baud_rate) << std::endl;
    settings.baud_rate = 115201;
    REQUIRE(!ep0.set(settings));
    settings.baud_rate = 115200;
    REQUIRE(ep0.set(settings));
    REQUIRE(ep0.get(settings));
    std::cout << C(settings.baud_rate) << std::endl;

    serial::Endpoint ep1{"/dev/ttyUSB1", settings};
    REQUIRE(ep1.valid());

    for (auto i = 0; i < 100000; ++i)
    {
        size_t offset = 0;
        REQUIRE(ep0.send(offset, "abc", 3));
        REQUIRE(offset == 3);
        char buffer[4];
        buffer[3] = 0;
        offset = 0;
        REQUIRE(ep1.receive(offset, buffer, 3));
        std::cout << buffer;
        std::flush(std::cout);
    }
    std::cout << std::endl;

    ep0.close();
    REQUIRE(!ep0.valid());
}

TEST_CASE("serial::Endpoint test", "[manual][serial][stream]")
{
    serial::Settings settings;
    settings.baud_rate = 9600;
    settings.stop_bits = 2;
    settings.parity = serial::Parity::None;
    serial::Endpoint ep{"/dev/ttyUSB0", settings};

    auto send_tp = std::chrono::system_clock::now();

    for (; true; )
    {
        if (true)
        {
            size_t offset = 0;
            char buffer[2];
            ep.receive(offset, buffer, 2);
            for (auto ix = 0; ix < offset; ++ix)
                std::cout << "Received " << std::hex << (int)buffer[ix] << std::endl;
        }
        if (true)
        {
            const auto now = std::chrono::system_clock::now();
            if (send_tp <= now)
            {
                size_t offset = 0;
                ep.send(offset, "\x1\x3", 2);
                send_tp += std::chrono::milliseconds(100);
            }
        }
    }
}
