#include <gubg/Logger.hpp>
#include <fstream>
#include <iostream>

namespace gubg { 
	std::ofstream devnull;

	std::ostream &Logger::os(int level)
	{
		return (this->level >= level ? std::cout : devnull);
	}
	std::ostream &Logger::error()
	{
		return std::cout << "Error: ";
	}
	std::ostream &Logger::warning()
	{
		return std::cout << "Warning: ";
	}
} 