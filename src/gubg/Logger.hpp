#ifndef HEADER_gubg_Logger_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Logger_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { 

	class Logger
	{
	public:
		int level = 0;

		std::ostream &os(int level);
		std::ostream &error();
		std::ostream &warning();
	};

} 

#endif