#include "catch.hpp"
#include "gubg/file/Descriptor.hpp"
#include <thread>
using namespace gubg::file;
using namespace gubg::mss;

namespace 
{
    auto logns = "test";

    class MySelect: public Select
    {
        public:
            virtual void select_ready(Descriptor d, EventType et)
            {
                S(logns);L(d << " is ready for some action: " << to_hr(et));
                switch (et)
                {
                    case EventType::Open:
                        {
                            Descriptor c;
                            d.accept(c);
                            add(c, AccessMode::Read);
                        }
                        break;
                    case EventType::Read:
                        {
                            std::string str(1024, '?');
                            d.read(str);
                            L(str);
                        }
                        break;
                }
            }
        private:
    };
}
TEST_CASE("file::Descriptor mtests", "[mt]")
{
        auto d = Descriptor::listen(File("/dev/ttyACM0"), AccessMode::ReadWrite);
        REQUIRE(d.valid());
        MySelect s;
        s.add(d, AccessMode::Read);
        s.process(std::chrono::seconds(10));
        Descriptor c;
        d.accept(c);
        REQUIRE(c.valid());
        s.process(std::chrono::seconds(10));
}
TEST_CASE("file::Descriptor file", "[mt]")
{
    auto std_in = Descriptor::std_in();
    REQUIRE(std_in.valid());

    MySelect s;
    s.add(std_in, AccessMode::Read);
    s.process(std::chrono::seconds(10));
}
TEST_CASE("file::Descriptor socket", "[mt]")
{
    auto d = Descriptor::listen(1234);
    REQUIRE(d.valid());
    MySelect s;
    s.add(d, AccessMode::Read);
    for (int i = 0; i < 10; ++i)
    {
        S(logns);L(i);
        s.process(std::chrono::seconds(60));
    }
}
TEST_CASE("file::Descriptor connect", "[mt]")
{
    for (int i = 0; i < 10; ++i)
    {
        auto d = Descriptor::connect("192.168.205.123", 1234);
        REQUIRE(d.valid());

        const std::string msg(1024, '!');
        size_t s = 0;
        REQUIRE(is_ok(d.write(s, msg.substr(s))));
        while (false and s < msg.size())
        {
            size_t tmp;
            REQUIRE(is_ok(d.write(tmp, msg.substr(s))));
            s += tmp;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
