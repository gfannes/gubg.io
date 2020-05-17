#include <gubg/markdown/ast/Writer.hpp>
#include <gubg/mss.hpp>
#include <optional>

namespace gubg { namespace markdown { namespace ast { 

    struct Stat
    {
        unsigned int depth = 0;
        std::optional<std::string> delimiter;

        Stat inc() const
        {
            Stat res = *this;
            ++res.depth;
            return res;
        }
    };

    bool write_(std::ostream &os, const Forest &forest, const Stat &stat)
    {
        MSS_BEGIN(bool);
        for (const auto &node: forest.nodes)
        {
            if (stat.delimiter)
                os << *stat.delimiter;
            switch (node.value.type)
            {
                case Type::Text:
                    os << node.value.content;
                    MSS(node.childs.empty());
                    break;
                case Type::Title:
                    {
                        os << std::string(stat.depth+1, '#') << ' ' << node.value.content;
                        auto mystat = stat.inc();
                        MSS(write_(os, node.childs, mystat));
                        os << std::endl << std::endl;
                    }
                    break;
                case Type::List:
                    break;
                case Type::Link:
                    break;
            }
        }
        MSS_END();
    }

    bool write(std::ostream &os, const Forest &forest)
    {
        MSS_BEGIN(bool);
        Stat stat;
        MSS(write_(os, forest, stat));
        MSS_END();
    }

} } } 
