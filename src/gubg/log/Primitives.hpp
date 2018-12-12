#ifndef HEADER_gubg_log_Primitives_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_Primitives_hpp_ALREADY_INCLUDED

#include "gubg/platform.h"
#if GUBG_PLATFORM_OS_ARDUINO
#include "Arduino.h"
#else
#include <iostream>
#endif

namespace gubg { namespace log { 

#if GUBG_PLATFORM_OS_ARDUINO
    template <typename T>
    void print(const T &v)
    {
        Serial.print(v);
    }
    inline void println()
    {
        Serial.print("\r\n");
    }
#else
    template <typename T>
    void print(const T &v)
    {
        std::cout << v;
    }
    inline void println()
    {
        std::cout << std::endl;
    }
#endif

} } 

#endif
