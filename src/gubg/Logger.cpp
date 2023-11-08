#include <gubg/Logger.hpp>

#include <fstream>
#include <iostream>
#include <functional>

namespace gubg {
    std::ofstream devnull;

    Logger::Logger()
        : Logger(Config{})
    {
    }
    Logger::Logger(const Config &config)
    {
        if (config.cout)
            buffer_.ostreams_.emplace_back(std::cout);
        if (!config.filename.empty())
        {
            fo_.open(config.filename);
            if (fo_.is_open())
                buffer_.ostreams_.emplace_back(fo_);
        }
    }

    std::ostream &Logger::os(int level)
    {
        return (this->level >= level ? ostream_ : devnull);
    }
    std::ostream &Logger::error()
    {
        return ostream_ << "Error: ";
    }
    std::ostream &Logger::warning()
    {
        return ostream_ << "Warning: ";
    }

    // Privates
    int Logger::Buffer::sync()
    {
        for (const auto &ref : ostreams_)
            ref.get() << str();
        str("");
        return 0;
    }

} // namespace gubg
