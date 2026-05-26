#ifndef HEADER_gubg_Logger_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Logger_hpp_ALREADY_INCLUDED

#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>

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

        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        ~Logger();

        int level = 0;
        bool to_file(std::optional<std::string_view> filename);

        std::ostream &os(std::optional<int> level);
        std::ostream &error();
        std::ostream &warning();

    private:
        class Buffer : public std::stringbuf
        {
        public:
            bool cout = false;
            std::optional<std::ofstream> fo;

            int sync() override;
        };

        Buffer buffer_;
        std::ostream ostream_{&buffer_};
    };

} // namespace gubg

#endif
