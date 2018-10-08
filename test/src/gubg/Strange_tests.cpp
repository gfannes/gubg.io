#include "catch.hpp"
#include "gubg/Strange.hpp"
using namespace gubg;

TEST_CASE("gubg::Strange default ctor tests", "[ut][Strange]")
{
	Strange r;
	SECTION("default ctor should be empty")
	{
		REQUIRE(r.empty());
		REQUIRE(0 == r.size());
		REQUIRE(r.str().empty());
	}
	SECTION("copying")
	{
		Strange rr(r);
		REQUIRE(rr.empty());
		REQUIRE(0 == rr.size());
	}
	SECTION("assignment")
	{
		Strange rr;
		rr = r;
		REQUIRE(rr.empty());
		REQUIRE(0 == rr.size());
	}
	SECTION("self assignment")
	{
		r = r;
		REQUIRE(r.empty());
		REQUIRE(0 == r.size());
	}
	SECTION("clear()")
	{
		REQUIRE(r.empty());
		r.clear();
		REQUIRE(r.empty());
	}
}
TEST_CASE("gubg::Strange creation from std::string == abc tests", "[ut][Strange]")
{
	std::string str("abc");

	SECTION("ctor")
	{
		Strange r(str);
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}
	SECTION("assignment")
	{
		Strange r;
		r = str;
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}

	Strange r(str);
	SECTION("assignment")
	{
		Strange rr;
		rr = r;
		REQUIRE(!rr.empty());
		REQUIRE(str.size() == rr.size());
	}
	SECTION("self assignment")
	{
		r = r;
		REQUIRE(!r.empty());
		REQUIRE(str.size() == r.size());
	}
	SECTION("str()")
	{
		REQUIRE(str == r.str());
	}
	SECTION("front()")
	{
		REQUIRE('a' == r.front());
	}
	SECTION("back()")
	{
		REQUIRE('c' == r.back());
	}
	SECTION("clear()")
	{
		REQUIRE(!r.empty());
		r.clear();
		REQUIRE(r.empty());
	}
	SECTION("pop_all() into Strange")
	{
		Strange rr;
		REQUIRE(!r.empty());
		r.pop_all(rr);
		REQUIRE(r.empty());
		REQUIRE(!rr.empty());
	}
	SECTION("pop_all() into std::string")
	{
		std::string ss;
		REQUIRE(!r.empty());
		r.pop_all(ss);
		REQUIRE(r.empty());
		REQUIRE(!ss.empty());
	}
	SECTION("pop_to()")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.pop_to(rr, 'a'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("b")
		{
			REQUIRE(r.pop_to(rr, 'b'));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
		SECTION("c")
		{
			REQUIRE(r.pop_to(rr, 'c'));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("e")
		{
			REQUIRE(!r.pop_to(rr, 'e'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("bc")
		{
			REQUIRE(r.pop_to(rr, "bc"));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
	}
    SECTION("diff_to() with Strange")
    {
        Strange sp = r;
        SECTION("when r is not changed, sp should diff to empty")
        {
            REQUIRE(sp.diff_to(r));
            REQUIRE(sp.empty());
        }
        SECTION("when r has one char popped, sp should diff to that char")
        {
            r.pop_front();
            REQUIRE(sp.diff_to(r));
            REQUIRE(sp.str() == "a");
        }
        SECTION("when r is empty, sp should not be changed: we assume r just ran out")
        {
            r.clear();
            REQUIRE(sp.diff_to(r));
            REQUIRE(sp.str() == "abc");
        }
        SECTION("when sp is empty, we expect failure")
        {
            sp.clear();
            REQUIRE(!sp.diff_to(r));
        }
        SECTION("is sp was popped, we expect a failure")
        {
            sp.pop_front();
            REQUIRE(!sp.diff_to(r));
        }
        SECTION("the end of r is not checked")
        {
            r.pop_back();
            REQUIRE(sp.diff_to(r));
            REQUIRE(sp.empty());
        }
        SECTION("the end of sp is not checked")
        {
            sp.pop_back();
            sp.pop_back();
            r.pop_front();
            r.pop_front();
            REQUIRE(sp.str() == "a");
            REQUIRE(sp.diff_to(r));
            REQUIRE(sp.str() == "ab");
        }
    }
	SECTION("pop_until() exclusive")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.pop_until(rr, 'a'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "bc");
		}
		SECTION("b")
		{
			REQUIRE(r.pop_until(rr, 'b'));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "c");
		}
		SECTION("c")
		{
			REQUIRE(r.pop_until(rr, 'c'));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "");
		}
		SECTION("e")
		{
			REQUIRE(!r.pop_until(rr, 'e'));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("ab")
		{
			REQUIRE(r.pop_until(rr, "ab"));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "c");
		}
		SECTION("bc")
		{
			REQUIRE(r.pop_until(rr, "bc"));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "");
		}
		SECTION("BC")
		{
			REQUIRE(!r.pop_until(rr, "BC"));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("pop_until() inclusive")
	{
		Strange rr;
		SECTION("a")
		{
			REQUIRE(r.pop_until(rr, 'a', true));
			REQUIRE(rr.str() == "a");
			REQUIRE(r.str() == "bc");
		}
		SECTION("b")
		{
			REQUIRE(r.pop_until(rr, 'b', true));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("c")
		{
			REQUIRE(r.pop_until(rr, 'c', true));
			REQUIRE(rr.str() == "abc");
			REQUIRE(r.str() == "");
		}
		SECTION("e")
		{
			REQUIRE(!r.pop_until(rr, 'e', true));
			REQUIRE(rr.empty());
			REQUIRE(r.str() == "abc");
		}
		SECTION("ab")
		{
			REQUIRE(r.pop_until(rr, "ab", true));
			REQUIRE(rr.str() == "ab");
			REQUIRE(r.str() == "c");
		}
		SECTION("bc")
		{
			REQUIRE(r.pop_until(rr, "bc", true));
			REQUIRE(rr.str() == "abc");
			REQUIRE(r.str() == "");
		}
		SECTION("BC")
		{
			REQUIRE(!r.pop_until(rr, "BC", true));
			REQUIRE(rr.str() == "");
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("pop_if")
	{
		REQUIRE(!r.pop_if('b'));
		REQUIRE(r.pop_if('a'));
		REQUIRE(r.pop_if('b'));
		REQUIRE(r.pop_if('c'));
		REQUIRE(!r.pop_if('e'));
	}
	SECTION("pop_back_if")
	{
		REQUIRE(!r.pop_back_if('b'));
		REQUIRE(r.pop_back_if('c'));
		REQUIRE(r.pop_back_if('b'));
		REQUIRE(r.pop_back_if('a'));
		REQUIRE(!r.pop_back_if('e'));
	}
	SECTION("pop_front")
	{
		REQUIRE(r.pop_front());
        REQUIRE(r.front() == 'b');
		REQUIRE(r.pop_front());
        REQUIRE(r.front() == 'c');
		REQUIRE(r.pop_front());
        REQUIRE(r.empty());
		REQUIRE(!r.pop_front());
	}
	SECTION("pop_back")
	{
		REQUIRE(r.pop_back());
        REQUIRE(r.back() == 'b');
		REQUIRE(r.pop_back());
        REQUIRE(r.back() == 'a');
		REQUIRE(r.pop_back());
        REQUIRE(r.empty());
		REQUIRE(!r.pop_back());
	}
	SECTION("pop_char")
	{
		char ch;
		REQUIRE(r.pop_char(ch));
		REQUIRE('a' == ch);
		REQUIRE(r.pop_char(ch));
		REQUIRE('b' == ch);
		REQUIRE(r.pop_char(ch));
		REQUIRE('c' == ch);
		REQUIRE(!r.pop_char(ch));
	}
	SECTION("pop_string")
	{
		SECTION("")
		{
			REQUIRE(r.pop_if(""));
			REQUIRE(r.str() == "abc");
		}
		SECTION("a")
		{
			REQUIRE(r.pop_if("a"));
			REQUIRE(r.str() == "bc");
		}
		SECTION("abc")
		{
			REQUIRE(r.pop_if("abc"));
			REQUIRE(r.str() == "");
		}
		SECTION("A")
		{
			REQUIRE(!r.pop_if("A"));
			REQUIRE(r.str() == "abc");
		}
	}
	SECTION("pop_line")
	{
		Strange line;
		SECTION("abc")
		{
			REQUIRE(r.pop_line(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(r.empty());
			REQUIRE(!r.pop_line(line));
		}
		SECTION("abc\n")
		{
			std::string str = "abc\n";
			r = str;
			REQUIRE(r.pop_line(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(r.empty());
			REQUIRE(!r.pop_line(line));
		}
		SECTION("abc\ndef")
		{
			std::string str = "abc\ndef";
			r = str;
			REQUIRE(r.pop_line(line));
			REQUIRE(line.str() == "abc");
			REQUIRE(!r.empty());
			REQUIRE(r.pop_line(line));
			REQUIRE(line.str() == "def");
		}
	}
}
TEST_CASE("gubg::Strange number tests", "[ut][Strange]")
{
	Strange r;
	SECTION("pop_decimal()")
	{
		struct TV
		{
			std::string str;
			long nbr;
		};
		std::vector<TV> tvs = {
			{"0", 0},
			{"1", 1},
			{"42", 42},
			{"1000000", 1000000},
		};
		for (auto tv: tvs)
		{
			{
				long nbr;
				r = tv.str;
				REQUIRE(r.pop_decimal(nbr));
				REQUIRE(tv.nbr == nbr);

				std::string str = "-"; str += tv.str;
				r = str;
				REQUIRE(r.pop_decimal(nbr));
				REQUIRE(-tv.nbr == nbr);
			}
			{
				int nbr;
				r = tv.str;
				REQUIRE(r.pop_decimal(nbr));
				REQUIRE(tv.nbr == nbr);

				std::string str = "-"; str += tv.str;
				r = str;
				REQUIRE(r.pop_decimal(nbr));
				REQUIRE(-tv.nbr == nbr);
			}
		}
	}
	SECTION("pop_float()")
	{
		std::string flt = "-42.123";
		r = flt;
		double d;
		REQUIRE(r.pop_float(d));
		REQUIRE(-42.123 == d);
	}
}
TEST_CASE("gubg::Strange construction and content lifetime tests", "[ut][Strange]")
{
    SECTION("const char *")
    {
        Strange strange("abc");
        REQUIRE(strange.str() == "abc");
    }
    SECTION("const char * and len")
    {
        Strange strange("abc", 2);
        REQUIRE(strange.str() == "ab");
    }
    SECTION("const std::string &")
    {
        const std::string str("abc");
        Strange strange(str);
        REQUIRE(strange.str() == "abc");
    }
    SECTION("std::string &&")
    {
        std::string str("abc");
        Strange strange(std::move(str));
        REQUIRE(strange.str() == "abc");
    }
    SECTION("dead std::string")
    {
        std::unique_ptr<std::string> str{new std::string("abc")};
        Strange strange(*str);
        str.reset();
        REQUIRE(strange.str() == "abc");
    }
    SECTION("out of scope")
    {
        Strange strange;
        {
            std::string str("abc");
            strange = str;
        }
        REQUIRE(strange.str() == "abc");
    }
}
TEST_CASE("gubg::Strange position tests", "[ut][Strange]")
{
    struct Scn
    {
        std::string content;
        unsigned int pop_count = 0;
    };
    struct Exp
    {
        gubg::Strange::Position position;
    };

    Scn scn;

    Exp exp;

    SECTION("default")
    {
    }
    SECTION("abc, 0 pop")
    {
        scn.content = "abc";
        scn.pop_count = 0;
    }
    SECTION("abc, 1 pop")
    {
        scn.content = "abc";
        scn.pop_count = 1;
        exp.position.ix = 1;
        exp.position.column = 1;
    }
    SECTION("abc, 3 pop")
    {
        scn.content = "abc";
        scn.pop_count = 3;
        exp.position.ix = 3;
        exp.position.column = 3;
    }
    SECTION("a\nbc, 3 pop")
    {
        scn.content = "a\nbc";
        scn.pop_count = 3;
        exp.position.ix = 3;
        exp.position.line = 1;
        exp.position.column = 1;
    }

    gubg::Strange strange(scn.content);
    REQUIRE(strange.pop_count(scn.pop_count));

    auto position = strange.position();
    REQUIRE(exp.position.ix == position.ix);
    REQUIRE(exp.position.line == position.line);
    REQUIRE(exp.position.column == position.column);
}
