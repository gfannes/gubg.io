#include "catch.hpp"
#include "gubg/t2/Parser.hpp"
#include <vector>
#include <list>
#include <iostream>
using namespace gubg;

namespace  { 
    struct Message
    {
        std::uint32_t tag = 0;

        Message() {}
        Message(std::uint32_t tag): tag(tag) {}
    };

    class Parser: public t2::Parser_crtp<Parser>
    {
    public:
        Message message;
        std::list<Message> messages;

        void t2_som()
        {
            message = Message{};
        }
        void t2_eom()
        {
            messages.push_back(message);
        }
        void t2_tag(std::uint32_t tag)
        {
            message.tag = tag;
        }
    private:
    };
} 

TEST_CASE("t2::Parser tests", "[ut][t2][Parser]")
{
    struct Scn
    {
        std::vector<std::uint8_t> data;
    };
    struct Exp
    {
        std::list<Message> messages;
    };

    Scn scn;
    Exp exp;

    SECTION("default") { }
    SECTION("som/eom")
    {
        scn.data.push_back(t2::md_som);
        SECTION("empty")
        {
            exp.messages.push_back(Message());
        }
        SECTION("0x3f")
        {
            scn.data.push_back(t2::md_open_tag | 0x3f);
            exp.messages.push_back(0x3f);
        }
        SECTION("0x40")
        {
            scn.data.push_back(t2::md_open_tag | 0x01);
            scn.data.push_back(t2::md_data | 0x00);
            exp.messages.push_back(0x40);
        }
        scn.data.push_back(t2::md_eom);
    }

    Parser parser;
    for (auto byte: scn.data)
        REQUIRE(parser.process(byte));

    REQUIRE(parser.messages.size() == exp.messages.size());
}
