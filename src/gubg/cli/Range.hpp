#ifndef HEADER_gubg_cli_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cli_Range_hpp_ALREADY_INCLUDED

#include <string>
#include <optional>

namespace gubg { namespace cli { 

    class Range
    {
    public:
        template <typename Int>
        Range(Int argc, const char **argv): argc_(argc), argv_(argv) {}

        bool pop(std::string &str);
        bool pop(int &i);
        bool pop(unsigned int &ui);
        bool pop(bool &b);

        template <typename T>
        bool pop(std::optional<T> &opt)
        {
            if (argix_ >= argc_)
                return false;
            opt.emplace();
            return pop(*opt);
        }

    private:
        const unsigned int argc_;
        const char ** const argv_;
        unsigned int argix_ = 0;
    };

} }

#endif
