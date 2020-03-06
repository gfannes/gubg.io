#include <gubg/ip/tcp/Server.hpp>
#include <gubg/ip/tcp/Client.hpp>
#include <catch.hpp>
#include <thread>
#include <chrono>
using namespace gubg::ip;

TEST_CASE("ip::tcp ClientServer tests", "[ut][ip][tcp][ClientServer]")
{
    const auto server_ep = Endpoint::localhost(1234);
    tcp::Server server{server_ep};
    tcp::Client client{server_ep};

    using Clock = std::chrono::system_clock;

    tcp::Connection scon, ccon;
    
    for (
            const auto stop_tp = Clock::now()+std::chrono::milliseconds{200};
            Clock::now() < stop_tp;
            std::this_thread::sleep_for(std::chrono::milliseconds{10})
        )
    {
        std::cout << "Processing" << std::endl;

        if (!scon.established())
        {
            std::cout << "  accepting incoming connection" << std::endl;
            REQUIRE(server.process(scon) == ReturnCode::OK);
        }
        else
        {
            std::cout << "  scon: " << scon << std::endl;
            REQUIRE(scon.process());
            for (const auto &msg: scon.received_msgs)
                std::cout << "Received: " << msg << std::endl;
            scon.received_msgs.clear();
        }

        if (!ccon.established())
        {
            std::cout << "  connecting to server" << std::endl;
            REQUIRE(client.process(ccon) == ReturnCode::OK);
        }
        else
        {
            std::cout << "  ccon: " << ccon << std::endl;
            REQUIRE(ccon.send("abc"));
            REQUIRE(ccon.process());
        }
    }
}
