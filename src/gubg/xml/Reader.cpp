#include <gubg/xml/Reader.hpp>
#include <gubg/mss.hpp>
#include <gubg/file/Filesystem.hpp>

namespace gubg { namespace xml { 

	void Reader::clear()
	{
		prolog.reset();
	}

	bool Reader::from_file(const std::filesystem::path &fp)
	{
		MSS_BEGIN(bool);

		std::string content;
		MSS(gubg::file::read(content, fp));

		MSS(from_string(content));

		MSS_END();
	}

	bool Reader::from_string(const std::string &str)
	{
		MSS_BEGIN(bool, "");

		clear();
		xml_ = str;

		if (xml_.pop_if("<?xml "))
		{
			auto &p = prolog.emplace();

			std::string key, value;
			while (pop_attr_(key, value))
			{
				if (key == "version") p.version = value;
				else if (key == "encoding") p.encoding = value;
				else MSS(false, std::cout << "Unknown prolog attribute " << key << ":" << value << std::endl);
			}
			std::string dump;
			xml_.pop_until(dump, "?>");
		}

		while (pop_open_tag_())
		{
		}

		MSS_END();
	}

	bool Reader::pop_open_tag_()
	{
		MSS_BEGIN(bool, "");

		pop_whitespace_();
		MSS_Q(xml_.pop_if('<'));
		auto &node = node_stack_.emplace_back();
		MSS(xml_.pop_until_any(node.tag, " >"));
		L(C(node.tag));
		std::string key, value;
		while (xml_.pop_if(' '))
		{
			if (!pop_attr_(key, value))
				break;
			node.attributes.emplace_back(key, value);
		}

		MSS_END();
	}

	bool Reader::pop_attr_(std::string &key, std::string &value)
	{
		MSS_BEGIN(bool);

		pop_whitespace_();
		MSS_Q(!xml_.empty());
		MSS_Q(xml_.front() != '/' && xml_.front() != '>');
		MSS_Q(xml_.pop_until(key, '='));
		char quote;
		MSS(xml_.pop_char(quote));
		MSS(quote == '"' || quote == '\'');
		MSS(xml_.pop_until(value, quote));

		MSS_END();
	}

	void Reader::pop_whitespace_()
	{
		xml_.strip(" \t\n\r");
	}

} } 
