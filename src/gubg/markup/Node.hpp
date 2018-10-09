#ifndef HEADER_gubg_markup_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markup_Node_hpp_ALREADY_INCLUDED

#include "gubg/markup/Data.hpp"
#include <gubg/mss.hpp>
#include <variant>
#include <list>

namespace gubg { namespace markup { 

    template <typename Renderer>
    class Node
    {
    private:
        using Self = Node<Renderer>;

    public:
        Node(){}
        template <typename Data>
        Node(const Data &data): data_(data){}

        template <typename Data>
        Node &add(const Data &data)
        {
            childs_.emplace_back(data);
            return childs_.back();
        }

        bool render(std::ostream &os, Renderer &renderer) const
        {
            MSS_BEGIN(bool);

            MSS(render_(renderer, nullptr));

            MSS(render_(renderer, &os));

            MSS_END();
        }

    private:
        bool render_(Renderer &renderer, std::ostream *os, const Self *parent = nullptr) const
        {
            MSS_BEGIN(bool);
            if (!os)
            {
                MSS(std::visit([&](const auto &data){return renderer.open(render_info_, data, (!!parent ? &parent->render_info_ : nullptr));}, data_));
                for (const auto &child: childs_)
                {
                    MSS(child.render_(renderer, os, this));
                }
                MSS(std::visit([&](const auto &data){return renderer.close(render_info_, data);}, data_));
            }
            else
            {
                MSS(std::visit([&](const auto &data){return renderer.open(*os, render_info_, data);}, data_));
                for (const auto &child: childs_)
                {
                    MSS(child.render_(renderer, os, this));
                }
                MSS(std::visit([&](const auto &data){return renderer.close(*os, render_info_, data);}, data_));
            }
            MSS_END();
        }

        std::variant<Document, Section> data_;
        mutable typename Renderer::Info render_info_;
        std::list<Node> childs_;
    };

} } 

#endif
