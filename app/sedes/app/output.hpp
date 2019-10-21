#ifndef HEADER_app_output_hpp_ALREADY_INCLUDED
#define HEADER_app_output_hpp_ALREADY_INCLUDED

#include <iostream>

namespace app { 

    inline std::ostream &log() { return std::cout; }
    inline std::ostream &error() { return std::cout << "Error: "; }

} 

#endif
