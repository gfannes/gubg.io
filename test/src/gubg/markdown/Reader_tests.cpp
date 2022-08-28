#include <gubg/markdown/Reader.hpp>
#include <catch.hpp>
#include <sstream>
using namespace gubg;

TEST_CASE("markdown::Reader tests", "[ut][markdown][Reader]")
{
	struct Scn
	{
		std::string text;
	};
	struct Exp
	{
		std::optional<std::string> error;
		std::ostringstream repr;
	};

	Scn scn;
	Exp exp;

	SECTION("positive")
	{
		SECTION("empty text") { }
		SECTION("h1")
		{
			scn.text = "# my heading";
			exp.repr << "[heading](1:my heading){}";
		}
		SECTION("h2")
		{
			scn.text = "## my heading";
			exp.repr << "[heading](2:my heading){}";
		}
		SECTION("h1h1")
		{
			scn.text = "# A\n# B";
			exp.repr << "[heading](1:A){}[heading](1:B){}";
		}
		SECTION("h1h2")
		{
			scn.text = "# A\n## B";
			exp.repr << "[heading](1:A){[heading](2:B){}}";
		}
		SECTION("h1h3")
		{
			scn.text = "# A\n### B";
			exp.repr << "[heading](1:A){[heading](3:B){}}";
		}
		SECTION("b1")
		{
			scn.text = "* A";
			exp.repr << "[bullet](1:A){}";
		}
		SECTION("b1b1")
		{
			scn.text = "* A\n* B";
			exp.repr << "[bullet](1:A){}[bullet](1:B){}";
		}
		SECTION("b1b2 - textile")
		{
			scn.text = "* A\n** B";
			exp.repr << "[bullet](1:A){[bullet](2:B){}}";
		}
		SECTION("b1b2 - markdown space")
		{
			scn.text = "* A\n * B";
			exp.repr << "[bullet](1:A){[bullet](2:B){}}";
		}
		SECTION("b1b2 - markdown tab")
		{
			scn.text = "* A\n\t* B";
			exp.repr << "[bullet](1:A){[bullet](2:B){}}";
		}
		SECTION("h1b1h2")
		{
			scn.text = "# A\n* B\n## C";
			exp.repr << "[heading](1:A){[bullet](1:B){}[heading](2:C){}}";
		}
		SECTION("line")
		{
			scn.text = "line";
			exp.repr << "[line](line)";
		}
		REQUIRE(!exp.error);
	}
	// SECTION("negative")
	// {
	// 	REQUIRE(!!exp.error);
	// }

	markdown::Reader reader{scn.text};

	std::ostringstream repr;
	for (markdown::Reader::Item item; reader(item); )
	{
		switch (item.what)
		{
			case markdown::Reader::Item::HeadingOpen:
			repr << "[heading](" << item.level << ":" << item.text << "){";
			break;
			case markdown::Reader::Item::HeadingClose:
			repr << "}";
			break;

			case markdown::Reader::Item::BulletOpen:
			repr << "[bullet](" << item.level << ":" << item.text << "){";
			break;
			case markdown::Reader::Item::BulletClose:
			repr << "}";
			break;

			case markdown::Reader::Item::Line:
			repr << "[line](" << item.text << ")";
			break;
		}
	}

	if (reader.error)
		std::cout << "markdown::Reader.error: " << *reader.error;
	REQUIRE(reader.error == exp.error);
	if (!reader.error)
	{
		REQUIRE(repr.str() == exp.repr.str());
	}
}	