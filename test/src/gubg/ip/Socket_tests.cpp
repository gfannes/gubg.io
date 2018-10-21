#include "catch.hpp"
#include "gubg/ip/Socket.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/Range.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
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
        REQUIRE(socket.recvfrom(str, ep) == ip::ReturnCode::WouldBlock);
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
    for (auto ix = 0u; ix < 10; ++ix)
    {
        std::cout << ix << std::endl;

        //Send message b -> a each time
        {
            std::ostringstream oss;
            oss << "[message](ix:" << ix << ")(from:b)(to:a)";
            auto msg = oss.str();
            auto range = make_range(msg);
            REQUIRE(socket_b.sendto(range, ep_a) == ReturnCode::OK);
            std::cout << "B: sent message " << msg << std::endl;
        }

        //Receive a
        {
            std::string msg; msg.resize(100);
            switch (socket_a.recvfrom(msg, ep_b))
            {
                case ReturnCode::OK:
                    std::cout << "A: received message " << msg << std::endl;
                    {
                        std::ostringstream oss;
                        oss << "[message](ix:" << ix << ")(from:a)(to:b)";
                        auto msg = oss.str();
                        auto range = make_range(msg);
                        REQUIRE(socket_a.sendto(range, ep_b) == ReturnCode::OK);
                        std::cout << "A: sent message " << msg << std::endl;
                    }
                    break;
                case ReturnCode::WouldBlock:
                    break;
                default:
                    REQUIRE(false);
                    break;
            }
        }

        //Receive b
        {
            std::string msg; msg.resize(100);
            ip::Endpoint ep;
            switch (socket_b.recvfrom(msg, ep))
            {
                case ReturnCode::OK:
                    std::cout << "B: received message " << msg << std::endl;
                    /* REQUIRE(ep == ep_a); */
                    break;
                case ReturnCode::WouldBlock:
                    break;
                default:
                    REQUIRE(false);
                    break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
