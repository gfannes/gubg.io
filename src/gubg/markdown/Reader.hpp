#ifndef HEADER_gubg_markdown_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markdown_Reader_hpp_ALREADY_INCLUDED

#include <gubg/Strange.hpp>

#include <optional>
#include <string>
#include <vector>
#include <ostream>

namespace gubg { namespace markdown { 

	class Reader
	{
	public:
		struct Item
		{
			// Heading levels start at 1, Bullet levels at 0
			// A normal line will be threated as a Bullet level 0
			enum What {HeadingOpen, HeadingClose, BulletOpen, BulletClose};
			What what;
			unsigned int level;
			std::string text;

			void clear() {*this = Item{};}
			void heading(unsigned int level);
			void bullet(unsigned int level);
			void line(const std::string &text);
		};

		std::optional<std::string> error;

		Reader(){}
		Reader(std::string text){setup_(std::move(text));}

		void setup(std::string text){setup_(std::move(text));}

		bool operator()(Item &);

	private:
		gubg::Strange strange_;
		std::vector<Item> stack_;
		unsigned int close_count_ = 0;

		void setup_(std::string &&text);
		void error_(const std::string &msg);

		bool heading_(Item &item);
		bool bullet_(Item &item);

		bool stack_has_geq_(Item::What what, unsigned int level) const;
	};

	std::ostream &operator<<(std::ostream &, const Reader::Item::What &);
	std::ostream &operator<<(std::ostream &, const Reader::Item &);

} } 

#endif