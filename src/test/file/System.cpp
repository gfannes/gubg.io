#include "catch.hpp"
#include "gubg/file/System.hpp"
#include <iostream>

namespace  { 
    using path = std::filesystem::path;
    bool do_print(const path &fn)
    {
        std::cout << fn << std::endl;
        return true;
    }
} 

TEST_CASE("gubg::file::System#each_entry() tests", "[gubg][file]")
{
    SECTION("in current_path")
    {
        REQUIRE(gubg::file::each_entry(do_print));
    }
    SECTION("in gubg.io")
    {
        REQUIRE(gubg::file::each_entry(do_print, "gubg.io"));
    }
}

TEST_CASE("gubg::file::System#each_recursive() tests", "[gubg][file]")
{
    SECTION("skip hidden files and dirs")
    {
        REQUIRE(gubg::file::each_recursive(do_print));
    }
    SECTION("do not skip hidden files and dirs")
    {
        REQUIRE(gubg::file::each_recursive(do_print, std::filesystem::current_path(), false));
    }
}

TEST_CASE("gubg::file::System#each_regex(pattern) tests", "[gubg][file]")
{
    REQUIRE(gubg::file::each_regex(".*/gubg/.*\\.[hc]pp", do_print));
}

TEST_CASE("gubg::file::System#each_glob(pattern) tests", "[gubg][file]")
{
    REQUIRE(gubg::file::each_glob("**/gubg/*.[hc]pp", do_print));
}
