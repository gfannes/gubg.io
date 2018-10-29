#include "catch.hpp"
#include "gubg/t2/Builder.hpp"
#include "gubg/string/Buffer.hpp"
#include <vector>
#include <functional>
using namespace gubg;

TEST_CASE("t2::Builder tests", "[ut][t2][Builder]")
{
    using Buffer = std::vector<t2::Byte>;
    using String = string::Buffer<t2::Byte>;
    using Document = t2::Document<String>;

    struct Scn
    {
        unsigned int size = 0;
        std::function<void (Document &)> ftor;
    };
    struct Exp
    {
        bool ok = true;
        std::vector<t2::Byte> message;
    };

    Scn scn;
    Exp exp;

    SECTION("default")
    {
        exp.message.push_back(t2::md_som);
        exp.message.push_back(t2::md_eom);
    }
    SECTION("SOM/EOM")
    {
        scn.ftor = [](Document &doc){
        };
        exp.message.push_back(t2::md_som);
        exp.message.push_back(t2::md_eom);
    }
    SECTION("tag 0x3f")
    {
        scn.ftor = [](Document &doc){
            auto t0 = doc.tag(0x3f);
        };
        exp.message.push_back(t2::md_som);
        exp.message.push_back(t2::md_open_tag | 0x3f);
        exp.message.push_back(t2::md_close_block);
        exp.message.push_back(t2::md_eom);
    }
    SECTION("tag 0x40")
    {
        scn.ftor = [](Document &doc){
            auto t0 = doc.tag(0x40);
        };
        exp.message.push_back(t2::md_som);
        exp.message.push_back(t2::md_open_tag | 0x01);
        exp.message.push_back(t2::md_data | 0x00);
        exp.message.push_back(t2::md_close_block);
        exp.message.push_back(t2::md_eom);
    }
    scn.size = exp.message.size();

    Buffer buffer;
    buffer.resize(scn.size);
    string::Buffer strbuf(buffer.data(), buffer.data()+buffer.size());

    Document doc(strbuf);

    if (scn.ftor)
        scn.ftor(doc);
    REQUIRE(doc.commit() == exp.ok);
    REQUIRE(strbuf.size() == exp.message.size());
    for (auto ix = 0u; ix < strbuf.size(); ++ix)
    {
        std::cout << C(ix) << std::endl;
        REQUIRE(int(strbuf[ix]) == int(exp.message[ix]));
    }
}
