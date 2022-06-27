#ifndef HEADER_gubg_xml_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Reader_hpp_ALREADY_INCLUDED

#include <gubg/Strange.hpp>
#include <string>
#include <filesystem>
#include <optional>
#include <vector>

namespace gubg { namespace xml { 

	class Reader
	{
	public:
		void clear();

		bool from_file(const std::filesystem::path &);
		bool from_string(const std::string &);

		struct Prolog
		{
			std::string version;
			std::string encoding;
		};
		std::optional<Prolog> prolog;

	private:
		bool pop_attr_(std::string &key, std::string &value);
		void pop_whitespace_();
		bool pop_open_tag_();

		struct NodeDescr
		{
			std::string tag;
			using Attribute = std::pair<std::string, std::string>;
			std::vector<Attribute> attributes;
		};
		std::vector<NodeDescr> node_stack_;

		gubg::Strange xml_;
	};

} } 

#endif