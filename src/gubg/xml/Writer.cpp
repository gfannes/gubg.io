#include <gubg/xml/Writer.hpp>

namespace gubg { namespace xml { 

	Writer::Tag Writer::tag(const std::string &name)
	{
		Tag t{os_, name};
		return std::move(t);
	}

} } 