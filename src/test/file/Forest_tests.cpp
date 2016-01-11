#include "catch.hpp"
#include "gubg/file/Forest.hpp"
using namespace gubg::file;
using namespace gubg::mss;

TEST_CASE("file::Forest tests", "[ut]")
{
    S("test");
    Forest forest;
    REQUIRE(is_ok(forest.add(File("/home/geert/gubg/gubg.io"), {"cpp", "hpp"})));
    L("\nAll files");
    for (auto f: forest.allFiles())
        L(f.name());
    L("\nAll roots");
    for (auto r: forest.allRoots())
        L(r.name());

    File resolved;
    REQUIRE(is_ok(forest.resolve(resolved, File("gubg.io/src/test/file/Forest_tests.cpp"))));
    L(resolved.name());
}
