#ifndef HEADER_gubg_xml_Escaper_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Escaper_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace xml { 

	class Escaper
	{
	public:
		const std::string &escape(const std::string &);
		const std::string &unescape(const std::string &);

	private:
		std::string tmp_;
	};

} } 

#endif