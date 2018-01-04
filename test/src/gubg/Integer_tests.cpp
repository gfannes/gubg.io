#include "catch.hpp"
#include "gubg/parse/numbers/Integer.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;
using namespace gubg::parse::numbers;

TEST_CASE("parse::number::Integer tests", "[ut]")
{
    auto a = "1234567890bla";
    long aa = 1234567890, bb = 0;
    size_t l = 13;
    REQUIRE(read(bb, a, l));
    REQUIRE(aa == bb);
    REQUIRE(l == 10);
    auto n = "-123";
    long nn = -123, mm = 0;
    REQUIRE(read(mm, n, l));
    REQUIRE(nn == mm);
}

TEST_CASE("parse::number::Integer stress tests", "[stress]")
{
    const size_t Nr = 1000000;
    vector<long>   ls(Nr);
    iota(ls.begin(), ls.end(), 0);
    vector<string> strs(Nr);
    {
        ostringstream oss;
        transform(ls.begin(), ls.end(), strs.begin(), [&](long l){oss.str(""); oss << l; return oss.str();});
    }

    long bb;
    size_t s;
    long sum_mine = 0, sum_strtol = 0;
    {
        for (size_t i = 0; i < Nr; ++i)
        {
            read(bb, strs[i].c_str(), s = strs[i].size());
            sum_mine += bb;
        }
    }
    {
        char *e;
        for (size_t i = 0; i < Nr; ++i)
        {
            bb = std::strtol(strs[i].c_str(), &e, 10);
            sum_strtol += bb;
        }
    }
    REQUIRE(sum_mine == sum_strtol);
}
