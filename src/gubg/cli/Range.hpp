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

        bool pop(std::string &str)
        {
            if (argix_ >= argc_)
                return false;
            str = argv_[argix_++];
            return true;
        }
        bool pop(int &i)
        {
            if (argix_ >= argc_)
                return false;

            try { i = std::stol(argv_[argix_]); }
            catch (...) {return false;}
                
            ++argix_;

            return true;
        }
        bool pop(unsigned int &ui)
        {
            if (argix_ >= argc_)
                return false;

            try { ui = std::stoul(argv_[argix_]); }
            catch (...) {return false;}

            ++argix_;

            return true;
        }

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
