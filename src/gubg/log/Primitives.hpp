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
    class Primitive
    {
    public:
        Primitive(): hws_(&Serial) {}

        template <typename Target>
        void setup(Target &tgt)
        {
            hws_ = &tgt;
        }

        template <typename T>
        void print(const T &v)
        {
            hws_->print(v);
        }
        inline void println()
        {
            hws_->print("\r\n");
        }
    private:
        HardwareSerial *hws_ = nullptr;
    };
#else
    class Primitive
    {
    public:
        Primitive(): os_(&std::cout) {}

        template <typename Target>
        void setup(Target &tgt)
        {
            os_ = &tgt;
        }

        template <typename T>
        void print(const T &v)
        {
            (*os_) << v;
        }
        inline void println()
        {
            (*os_) << std::endl;
        }
    private:
        std::ostream *os_ = nullptr;
    };
#endif

} } 

#endif
