#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include <list>
#include <algorithm>
#include <cassert>

namespace gubg { namespace filesystem {

    static const char *logns = nullptr;

    bool get_from_to(std::filesystem::path & result, const std::filesystem::path & from, const std::filesystem::path & to)
    {
        MSS_BEGIN(bool, logns);
        L(C(from)C(to));
        MSS(from.is_absolute());
        MSS(to.is_absolute());

        using range = gubg::Range<std::filesystem::path::iterator>;
        std::filesystem::path f = normalize(from);
        std::filesystem::path t = normalize(to);
        L(C(f)C(t));

        if(from.root_path() != to.root_path())
        {
            result = to;
            L("Root paths are not the same" << C(result));
            MSS_RETURN_OK();
        }

        auto pop_equal_fronts = [&](range & lhs, range & rhs)
        {
            while(!lhs.empty() && !rhs.empty())
            {
                if (lhs.front() != rhs.front())
                    return;

                lhs.pop_front();
                rhs.pop_front();
            }
        };

        // remove the front part which is the same
        auto rf = gubg::make_range(f);
        auto rt = gubg::make_range(t);
        pop_equal_fronts(rf, rt);

        // start at from
        result.clear();

        // go back till different part
        std::for_each(RANGE(rf), [&](const auto &) { result /= ".."; });

        // add the new part
        std::for_each(RANGE(rt), [&](const auto & v) { result /= v; });

        L("Could make a shortcut" << C(result));

        MSS_END();
    }

    std::filesystem::path get_relative_to(const std::filesystem::path & from, const std::filesystem::path & to)
    {
        using range = gubg::Range<std::filesystem::path::iterator>;

        // from absolute and to relative -> to
        // from relative and to absolute -> to
        if (from.is_absolute() != to.is_absolute())
            return to;



        std::filesystem::path f = normalize(from);
        std::filesystem::path t = normalize(to);

        auto pop_equal_fronts = [&](range & lhs, range & rhs)
        {
            while(!lhs.empty() && !rhs.empty())
            {
                if (lhs.front() != rhs.front())
                    return;

                lhs.pop_front();
                rhs.pop_front();
            }
        };

        // remove the front part which is the same
        auto rf = gubg::make_range(f);
        auto rt = gubg::make_range(t);
        pop_equal_fronts(rf, rt);

        // start at from
        std::filesystem::path result;

        // go back till different part
        std::for_each(RANGE(rf), [&](const auto &) { result /= ".."; });

        // add the new part
        std::for_each(RANGE(rt), [&](const auto & v) { result /= v; });

        return result;
    }

    std::filesystem::path normalize(const std::filesystem::path & src)
    {
        S(logns);
        std::list<std::filesystem::path::iterator> to_add;
        std::filesystem::path result;

        for(auto it = src.begin(); it != src.end(); ++it)
        {
            const auto &el = *it;
            L(C(el));
            if (false) {}
            else if (el == "..")
            {
                // can we pop a position ?
                if (!to_add.empty())
                    // yes, so remove this
                    to_add.pop_back();
                else
                    // no, so add the it to the result
                    result /= el;
            }
            else if (!el.empty() && el != ".")
            {
                to_add.push_back(it);
            }
        }

        // add all the remaining
        for(auto it : to_add)
            result /= *it;

        return result;
    }


} }
