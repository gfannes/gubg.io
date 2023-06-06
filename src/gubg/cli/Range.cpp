#include <gubg/cli/Range.hpp>
#include <cstring>

namespace gubg { namespace cli {

    bool Range::pop(std::string &str)
    {
        if (argix_ >= argc_)
            return false;
        str = argv_[argix_++];
        return true;
    }

    bool Range::pop(int &i)
    {
        if (argix_ >= argc_)
            return false;

        try
        {
            i = std::stol(argv_[argix_]);
        } catch (...)
        {
            return false;
        }

        ++argix_;

        return true;
    }

    bool Range::pop(unsigned int &ui)
    {
        if (argix_ >= argc_)
            return false;

        try
        {
            ui = std::stoul(argv_[argix_]);
        } catch (...)
        {
            return false;
        }

        ++argix_;

        return true;
    }

    bool Range::pop(bool &b)
    {
        if (argix_ >= argc_)
            return false;

        auto is = [&](const char *wanted) {
            return std::strcmp(argv_[argix_], wanted) == 0;
        };

        b = (false || is("y") || is("yes") || is("1") || is("true"));

        ++argix_;

        return true;
    }

}} // namespace gubg::cli
