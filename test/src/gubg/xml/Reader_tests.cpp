#include <gubg/xml/Reader.hpp>

#include <catch.hpp>

#include <string>
using namespace gubg;

TEST_CASE("xml::Reader tests", "[ut][xml][Reader]")
{
	struct Scn
	{
		std::string content;
	};
	struct Exp
	{
		std::string repr;
	};

	Scn scn;
	Exp exp;

	SECTION("empty")
	{
		exp.repr = "";
	}
	SECTION("prolog")
	{
		scn.content = "<?xml version='1.0' encoding=\"UTF-8\" ?>";
		exp.repr = "prolog @version:1.0 @encoding:UTF-8 ";
	}
	SECTION("escape")
	{
		scn.content = "<?xml &amp;&lt;&gt;&apos;&quot;='&amp;&lt;&gt;&apos;&quot;'?>";
		exp.repr = "prolog @&<>'\":&<>'\" ";
	}
	SECTION("<a/>")
	{
		scn.content = "<a/>";
		exp.repr = ">a <a ";
	}
	SECTION("<a></a>")
	{
		scn.content = "<a></a>";
		exp.repr = ">a <a ";
	}
	SECTION("<a aa='bb'/>")
	{
		scn.content = "<a aa='bb'/>";
		exp.repr = ">a @aa:bb <a ";
	}
	SECTION("<a><b/></a>")
	{
		scn.content = "<a><b/></a>";
		exp.repr = ">a >b <b <a ";
	}

	xml::Reader reader{scn.content};

	std::string repr;
	for (xml::Reader::Item item; reader(item); )
	{
		using Item = xml::Reader::Item;
		switch (item.kind)
		{
			case Item::Unset:
			REQUIRE(false);
			break;

			case Item::Prolog:
			repr += "prolog ";
			for (const auto &[k,v]: item.attributes)
				repr += "@"+k+":"+v+" ";
			break;

			case Item::TagOpen:
			repr += ">" + item.tag+" ";
			for (const auto &[k,v]: item.attributes)
				repr += "@"+k+":"+v+" ";
			break;

			case Item::TagClose:
			repr += "<" + item.tag+" ";
			for (const auto &[k,v]: item.attributes)
				repr += "@"+k+":"+v+" ";
			break;
		}
	}
	if (reader.error)
		std::cout << *reader.error << std::endl;
	REQUIRE(!reader.error);
	REQUIRE(repr == exp.repr);
}	
