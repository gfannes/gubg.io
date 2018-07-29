#include "catch.hpp"
#include "gubg/s11n/Writer.hpp"
#include <optional>
#include <iostream>
using namespace gubg;

namespace  { 
    using RC = s11n::ReturnCode;

    class String
    {
    public:
        String(size_t max): max_(max) {}

        const std::string &str() const {return str_;}

        bool push_back(char ch)
        {
            if (str_.size() >= max_)
                return false;
            str_.push_back(ch);
            return true;
        }

    private:
        const size_t max_;
        std::string str_;
    };

    class A
    {
    public:
    private:
    };

    struct MyTag { };
} 

TEST_CASE("s11n::Writer tests", "[ut][s11n][Writer]")
{
    using Writer = s11n::Writer<MyTag, String>;

    struct Scn
    {
        size_t str_size = 0;
        std::optional<A> a;
        bool a_as_typed = true;
    };
    struct Exp
    {
        RC rc = RC::OK;
        std::string output;
    };
    Scn scn;
    Exp exp;

    SECTION("default")
    {
    }
    SECTION("A")
    {
        scn.a.emplace();
        SECTION("as typed")
        {
            scn.a_as_typed = true;
        }
        SECTION("as named")
        {
            scn.str_size += 3;
            scn.a_as_typed = false;
            exp.output = "[a]";
        }
    }

    String str(scn.str_size);
    Writer writer(str);

    RC rc = RC::OK;
    if (scn.a)
    {
        if (scn.a_as_typed)
        {
            AGG(rc, writer.typed(*scn.a));
        }
        else
        {
            AGG(rc, writer.named(*scn.a, "a"));
        }
    }
    REQUIRE(exp.rc == rc);
    std::cout << "***" << std::endl;
    std::cout << str.str() << std::endl;
    std::cout << "***" << std::endl;
    REQUIRE(exp.output == str.str());
}
