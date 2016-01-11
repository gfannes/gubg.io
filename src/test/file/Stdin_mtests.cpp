#include "catch.hpp"
#include "gubg/file/Descriptor.hpp"
using namespace gubg::file;

namespace 
{
    auto logns = "test";

    class MySelect: public Select
    {
        public:
                virtual void select_ready(Descriptor d, EventType et)
                {
                    S(logns);L(d << " is ready " << STREAM((int)et));
                }
        private:
    };
}
TEST_CASE("file::Stdin mtests", "[mt]")
{
    auto std_in = Descriptor::std_in();
    REQUIRE(std_in.valid());

    MySelect s;
    s.add(std_in, AccessMode::Read);
    s.process(std::chrono::seconds(10));
}
