#include "catch.hpp"
#include "gubg/string/Std.hpp"
#include "gubg/string/Buffer.hpp"
#include "gubg/mss.hpp"
#include <array>

using namespace gubg;

namespace  { 
    template <typename String>
    bool test(String &str, bool limited)
    {
        MSS_BEGIN(bool, "");
        MSS(str.empty());
        MSS(str.size() == 0);
        {
            auto ptr = str.grow(0);
            MSS(!!ptr);
            MSS(str.shrink(0));
            MSS(!str.shrink(1));
        }
        MSS(str.empty());
        MSS(str.size() == 0);
        {
            auto ptr = str.grow(1);
            MSS(!!ptr);
            MSS(str.shrink(1));
            MSS(!str.shrink(1));
        }
        MSS(str.empty());
        MSS(str.size() == 0);
        {
            auto ptr = str.grow(100);
            MSS(!!ptr);
            MSS(str.shrink(100));
        }
        MSS(str.empty());
        MSS(str.size() == 0);
        {
            auto ptr = str.grow(101);
            if (limited)
            {
                MSS(!ptr);
            }
            else
            {
                MSS(!!ptr);
                MSS(str.shrink(101));
            }
        }
        MSS(str.empty());
        MSS(str.size() == 0);
        MSS_END();
    }
} 

TEST_CASE("string tests", "[ut][string]")
{
    S("");
    SECTION("Std")
    {
        std::string str;
        string::Std string(str);
        REQUIRE(test(string, false));
    }
    SECTION("Buffer")
    {
        std::array<char, 100> ary;
        string::Buffer string(ary.data(), ary.data()+ary.size());
        REQUIRE(test(string, true));
    }
}
