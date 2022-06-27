#include <catch.hpp>
#include <gubg/xml/Reader.hpp>
#include <string>
using namespace gubg;

TEST_CASE("xml::Reader", "[ut][xml][Reader]")
{
	xml::Reader reader;
	REQUIRE(!reader.prolog);

	REQUIRE(reader.from_file("gubg.io/test/src/gubg/xml/prolog.xml"));
	REQUIRE(!!reader.prolog);
	REQUIRE(reader.prolog->version == "1.0");
	REQUIRE(reader.prolog->encoding == "UTF-8");
}	
