#include <gubg/Logger.hpp>

#include <fstream>
#include <iostream>
#include <streambuf>

// Implementation of `devnull`
namespace {
    class NullBuffer : public std::streambuf
    {
    protected:
        int_type overflow(int_type ch) override
        {
            return traits_type::not_eof(ch);
        }
        std::streamsize xsputn(const char *, std::streamsize n) override
        {
            return n;
        }
    };

    NullBuffer null_buffer;
    std::ostream devnull(&null_buffer);
} // namespace

namespace gubg {
    Logger::Logger()
        : Logger(Config{})
    {
    }
    Logger::Logger(const Config &config)
    {
        buffer_.cout = config.cout;
        if (!config.filename.empty())
            to_file(config.filename);
    }

    Logger::~Logger()
    {
        ostream_.flush();
    }

    bool Logger::to_file(std::optional<std::string_view> filename)
    {
        ostream_.flush();

        if (buffer_.fo)
            buffer_.fo.reset();

        if (filename)
        {
            buffer_.fo.emplace();
            buffer_.fo->open(std::string(*filename));
            if (!buffer_.fo->is_open())
            {
                buffer_.fo.reset();
                return false;
            }
        }

        return true;
    }

    std::ostream &Logger::os(std::optional<int> level)
    {
        const bool do_log = this->level >= level.value_or(this->level);
        return do_log ? ostream_ : devnull;
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
        if (cout || fo)
        {
            if (cout)
                std::cout << str() << std::flush;
            if (fo)
                *fo << str() << std::flush;
        }
        str("");
        return 0;
    }

} // namespace gubg
