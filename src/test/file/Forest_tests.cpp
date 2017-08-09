#include "catch.hpp"
#include "gubg/file/Forest.hpp"
using namespace gubg::file;
using namespace gubg::mss;

TEST_CASE("file::Forest tests", "[ut]")
{
    S("test");

    auto ptr = std::getenv("gubg");
    const std::string gubg_dir = (ptr ? ptr : "");
    REQUIRE(!gubg_dir.empty());

    Forest forest;
    REQUIRE(is_ok(forest.add(File(gubg_dir+"/gubg.io"), {"cpp", "hpp"})));
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
