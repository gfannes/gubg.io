#include "catch.hpp"
#include "gubg/ip/Socket.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/Range.hpp"
#include <iostream>
#include <chrono>
#include <thread>
using namespace gubg;

TEST_CASE("ip::Socket tests", "[ut][ip][Socket]")
{
    ip::Socket socket;
    SECTION("fresh")
    {
        REQUIRE(!socket.valid());
    }
    SECTION("ctor")
    {
        ip::Socket tmp(ip::Type::UDP, ip::Version::V4);
        std::cout << C(tmp) << std::endl;
        socket = std::move(tmp);
        std::cout << C(tmp) << std::endl;
        REQUIRE(socket.valid());
    }
    SECTION("bind")
    {
        REQUIRE(socket.setup(ip::Type::UDP, ip::Version::V4));
        ip::Endpoint ep(ip::Address{127,0,0,1}, 1234);
        REQUIRE(socket.bind(ep) == ip::ReturnCode::OK);
    }
    SECTION("sendto")
    {
        REQUIRE(socket.setup(ip::Type::UDP, ip::Version::V4));
        /* ip::Endpoint ep(ip::Address{127,0,0,1}, 1234); */
        ip::Endpoint ep(ip::Address{192,168,1,76}, 6789);
        std::array<char, 10> buffer{};
        buffer[0] = 'a';
        buffer[1] = 'b';
        buffer[2] = '\0';
        auto data = make_range(buffer);
        REQUIRE(socket.sendto(data, ep) == ip::ReturnCode::OK);
        REQUIRE(data.empty());
    }
    SECTION("recvfrom")
    {
        REQUIRE(socket.setup(ip::Type::UDP, ip::Version::V4));
        std::string str;
        str.resize(100);
        ip::Endpoint ep;
        bool blocking;
        REQUIRE(socket.get_blocking(blocking) == ip::ReturnCode::OK);
        REQUIRE(blocking);
        REQUIRE(socket.set_blocking(false) == ip::ReturnCode::OK);
        REQUIRE(socket.get_blocking(blocking) == ip::ReturnCode::OK);
        REQUIRE(!blocking);
        REQUIRE(socket.recvfrom(str, ep) == ip::ReturnCode::OK);
        REQUIRE(str.empty());
    }
    std::cout << socket << std::endl;
}

TEST_CASE("ip::Socket scenario", "[ut][ip][Socket][scn]")
{
    using namespace gubg::ip;

    Endpoint ep_a{Address::localhost(), 1234};
    Endpoint ep_b;
    Socket socket_a{Type::UDP, Version::V4};
    Socket socket_b{Type::UDP, Version::V4};

    REQUIRE(socket_a.bind(ep_a) == ReturnCode::OK);
    for (auto i = 0u; i < 10; ++i)
    {
        std::cout << i << std::endl;
        switch (i)
        {
            case 0:
                {
                    std::string msg = "abc";
                    auto range = make_range(msg);
                    REQUIRE(socket_b.sendto(range, ep_a) == ReturnCode::OK);
                    std::cout << "sent message " << msg << std::endl;
                }
                break;
            case 1:
                {
                    std::string msg; msg.resize(100);
                    REQUIRE(socket_a.recvfrom(msg,ep_b) == ReturnCode::OK);
                    std::cout << "received message " << msg << std::endl;
                }
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
