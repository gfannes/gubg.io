#include "catch.hpp"
#include "gubg/t2/Parser.hpp"
#include "gubg/zip.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <list>
#include <optional>
#include <iostream>
using namespace gubg;

namespace  { 
    using Data = t2::Data;

    struct Message
    {
        std::optional<Data> tag;
        std::optional<Data> key;
        std::optional<Data> value;

        Message() {}
        Message(Data tag): tag(tag) {}
        Message(Data tag, Data key, Data value): tag(tag), key(key), value(value) {}
        bool operator==(const Message &rhs) const
        {
            return tag == rhs.tag && key == rhs.key && value == rhs.value;
        }
    };

    class Parser: public t2::Parser_crtp<Parser>
    {
    public:
        Message message;
        std::list<Message> messages;

        void t2_open(Data tag, unsigned int level)
        {
            std::cout << "opening " << C(tag)C(level) << std::endl;
            tags_.resize(level+1);
            tags_[level] = tag;
            if (level == 0)
            {
                std::cout << "SOM\n";
                message = Message{};
            }
            else
                message.tag = tag;
        }
        void t2_close(unsigned int level)
        {
            tags_.resize(level+1);
            std::cout << "closing " << C(tags_[level])C(level) << std::endl;
            if (level == 0)
            {
                std::cout << "EOM\n";
                messages.push_back(message);
            }
        }
        void t2_attr(Data key, Data value)
        {
            std::cout << C(key)C(value) << std::endl;
            message.key = key;
            message.value = value;
        }
    private:
        std::vector<Data> tags_;
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
        std::optional<Data> tag;
        std::optional<Data> key;
        std::optional<Data> value;
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
        SECTION("tag 0x3f")
        {
            scn.data.push_back(t2::md_open_tag | 0x3f);
            exp.messages.push_back(0x3f);
        }
        SECTION("tag 0x40")
        {
            scn.data.push_back(t2::md_open_tag | 0x01);
            scn.data.push_back(t2::md_data | 0x00);
            exp.messages.emplace_back(0x40);
        }
        SECTION("tag 0, key 1, value 2")
        {
            scn.data.push_back(t2::md_open_tag | 0x00);
            scn.data.push_back(t2::md_open_attr | 0x01);
            scn.data.push_back(t2::md_open_attr | 0x02);
            exp.messages.emplace_back(0x00, 0x01, 0x02);
        }
        scn.data.push_back(t2::md_eom);
    }

    Parser parser;
    for (auto byte: scn.data)
        REQUIRE(parser.process(byte));

    REQUIRE(parser.messages.size() == exp.messages.size());
    REQUIRE(gubg::zip(RANGE(parser.messages), exp.messages.begin(), [](const auto &parser_msg, const auto &exp_msg){ return parser_msg == exp_msg; }));
}
