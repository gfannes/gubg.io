#include <gubg/markdown/ast/Writer.hpp>
#include <gubg/mss.hpp>
#include <optional>
#include <vector>

namespace gubg { namespace markdown { namespace ast { 

    struct Stat
    {
        std::vector<unsigned int> path;
        std::optional<std::string> delimiter;
    };

    bool write_(std::ostream &os, const Forest &forest, const Stat &stat)
    {
        MSS_BEGIN(bool);

        auto path = stat.path;
        path.push_back(0);

        for (const auto &node: forest.nodes)
        {
            if (stat.delimiter)
                os << *stat.delimiter;

            switch (node.value.type)
            {
                case Type::Section:
                    {
                        auto next_stat = stat;
                        next_stat.path = path;
                        os << ">> section " << &node << std::endl;
                        MSS(write_(os, node.childs, next_stat));
                        os << "<< section " << &node << std::endl;
                        ++path.back();
                    }
                    break;
                case Type::Title:
                    {
                        os << std::string(stat.path.size()+1, '#') << ' ' << node.value.content;
                        MSS(write_(os, node.childs, stat));
                        os << std::endl << std::endl;
                    }
                    break;
                case Type::Text:
                    {
                        os << node.value.content;
                        MSS(node.childs.empty());
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
