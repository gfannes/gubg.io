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
        }

        if (!ccon.established())
        {
            std::cout << "  connecting to server" << std::endl;
            REQUIRE(client.process(ccon) == ReturnCode::OK);
        }
        else
        {
        }
    }
}
