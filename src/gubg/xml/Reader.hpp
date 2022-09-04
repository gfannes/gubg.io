#ifndef HEADER_gubg_xml_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Reader_hpp_ALREADY_INCLUDED

#include <gubg/Strange.hpp>

#include <string>
#include <vector>
#include <optional>

namespace gubg { namespace xml { 

	class Reader
	{
	public:
		struct Item
		{
			enum Kind {Unset, Prolog, TagOpen, TagClose};

			Kind kind = Kind::Unset;

			std::string tag;

			using Attribute = std::pair<std::string, std::string>;
			using Attributes = std::vector<Attribute>;
			Attributes attributes;
		};

		std::optional<std::string> error;

		Reader() {}
		Reader(const std::string &content) {setup(content);}

		void setup(const std::string &content);

		bool operator()(Item &item);

	private:
		struct Error
		{
			std::optional<std::string> &error;

			Error(std::optional<std::string> &error): error(error) {}

			void operator()(const std::string &msg);
		};
		Error error_{error};
		void pop_whitespace_();
		static void pop_whitespace_(gubg::Strange &);
		bool pop_prolog_(Item &);
		static bool pop_attribute_(Item::Attribute &, gubg::Strange &, Error &);
		static bool unescape_(std::string &, Error &);

		gubg::Strange strange_;
		std::vector<std::string> tag_stack_;
		bool emit_top_stack_ = false;
		bool check_prolog_ = true;
	};

} } 

#endif