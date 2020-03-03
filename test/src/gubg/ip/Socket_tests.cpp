#include <gubg/ip/Socket.hpp>
#include <gubg/macro/capture.hpp>
#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <catch.hpp>
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
        REQUIRE(socket.create(ip::Type::UDP, ip::Version::V4));
        ip::Endpoint ep(ip::Address{127,0,0,1}, 1234);
        REQUIRE(socket.bind(ep) == ip::ReturnCode::OK);
    }
    SECTION("sendto")
    {
        REQUIRE(socket.create(ip::Type::UDP, ip::Version::V4));
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
        REQUIRE(socket.create(ip::Type::UDP, ip::Version::V4));
        std::string str;
        str.resize(100);
        ip::Endpoint ep;
        REQUIRE(socket.is_blocking());
        socket.set_blocking(false);
        REQUIRE(!socket.is_blocking());
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

TEST_CASE("ip::Socket accept-connect", "[ut][ip][Socket][accept][connect]")
{
    auto create_socket = [](auto &socket)
    {
        MSS_BEGIN(bool);
        MSS(socket.create(ip::Type::TCP, ip::Version::V4));
        MSS(socket.reuse_address(true));
        socket.set_blocking(false);
        MSS_END();
    };

    ip::Socket server_acceptor, server_connection, client;
    REQUIRE(create_socket(server_acceptor));
    REQUIRE(create_socket(server_connection));
    REQUIRE(create_socket(client));

    ip::Endpoint server_ep{ip::Address{127,0,0,1}, 1234};
    std::cout << server_ep << std::endl;
    REQUIRE(server_acceptor.bind(server_ep) == ip::ReturnCode::OK);
    REQUIRE(server_acceptor.listen() == ip::ReturnCode::OK);

    ip::Endpoint client_ep;
    REQUIRE(server_acceptor.accept(server_connection, client_ep) == ip::ReturnCode::WouldBlock);
    REQUIRE(client.connect(server_ep) == ip::ReturnCode::WouldBlock);

    REQUIRE(server_acceptor.accept(server_connection, client_ep) == ip::ReturnCode::OK);
    std::cout << client_ep << std::endl;

    REQUIRE(client.is_connected() == ip::ReturnCode::OK);

    {
        const std::string send_request = "request";
        std::size_t offset = 1;
        REQUIRE(client.send(send_request.data(), send_request.size(), offset) == ip::ReturnCode::OK);
        std::cout << "Client: sent request \"" << send_request << "\"" << std::endl;
        REQUIRE(offset == send_request.size());
    }

    {
        std::string recv_request(1024, '?');
        std::size_t offset = 0;
        REQUIRE(server_connection.recv(&recv_request[0], recv_request.size(), offset) == ip::ReturnCode::OK);
        recv_request.resize(offset);
        std::cout << "Server: received request \"" << recv_request << "\"" << std::endl;

        std::string send_answer = recv_request;
        std::reverse(RANGE(send_answer));
        offset = 0;
        REQUIRE(server_connection.send(send_answer.data(), send_answer.size(), offset) == ip::ReturnCode::OK);
        std::cout << "Server: sent answer \"" << send_answer << "\"" << std::endl;
    }

    {
        std::string recv_answer(1024, '?');
        std::size_t offset = 0;
        REQUIRE(client.recv(&recv_answer[0], recv_answer.size(), offset) == ip::ReturnCode::OK);
        recv_answer.resize(offset);
        std::cout << "Client: received answer \"" << recv_answer << "\"" << std::endl;

        REQUIRE(recv_answer == "tseuqe");
    }
}
