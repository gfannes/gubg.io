#ifndef HEADER_gubg_Logger_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Logger_hpp_ALREADY_INCLUDED

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include <vector>

namespace gubg {

    class Logger
    {
    public:
        struct Config
        {
            bool cout = true;
            std::string filename;
        };
        Logger();
        Logger(const Config &);

        int level = 0;

        std::ostream &os(int level);
        std::ostream &error();
        std::ostream &warning();

    private:
        class Buffer : public std::stringbuf
        {
        public:
            std::vector<std::reference_wrapper<std::ostream>> ostreams_;
            int sync() override;
        };
        std::ofstream fo_;
        Buffer buffer_;
        std::ostream ostream_{&buffer_};
    };

} // namespace gubg

#endif
