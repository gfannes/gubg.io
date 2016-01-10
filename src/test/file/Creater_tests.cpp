#include "catch.hpp"
#include "gubg/file/Creater.hpp"
using namespace gubg::file;
using namespace gubg::mss;

namespace 
{
    class Creater: public gubg::file::Creater_crtp<Creater>
    {
        public:
            template <typename Files, typename SI>
            ReturnCode creater_create(Files files, SI si) const
            {
                MSS_BEGIN(ReturnCode);
                for (auto f: files)
                    MSS(write(si, f));
                MSS_END();
            }
        private:
    };
}

TEST_CASE("file::Creater tests", "[ut]")
{
    Creater creater;
    REQUIRE(is_ok(creater.setCache(File("/a/b/c"))));
    REQUIRE(is_ok(creater.setCache(File("/tmp"))));
    creater({File("/home/gfannes/blabla")}, "abc");
}
