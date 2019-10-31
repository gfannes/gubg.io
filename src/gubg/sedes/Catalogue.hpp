#ifndef HEADER_gubg_sedes_Catalogue_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Catalogue_hpp_ALREADY_INCLUDED

#include <gubg/sedes/Struct.hpp>
#include <gubg/graph/TopologicalOrder.hpp>
#include <gubg/mss.hpp>
#include <gubg/OnlyOnce.hpp>
#include <optional>
#include <string>
#include <map>

namespace gubg { namespace sedes { 

    class Catalogue
    {
    public:
        bool add(const Struct &struc)
        {
            MSS_BEGIN(bool);
            MSS(name__struct_.count(struc.name) == 0);
            name__struct_[struc.name] = struc;
            order_.reset();
            MSS(create_order_());
            MSS_END();
        }

        template <typename Ftor>
        void each(Ftor &&ftor)
        {
            if (!create_order_())
                return;

            for (auto ptr: order_->order)
            {
                std::cout << ptr << std::endl;
                ftor(*ptr);
            }
        }

    private:
        bool create_order_()
        {
            MSS_BEGIN(bool);

            if (order_)
                return true;

            order_.emplace();
            auto each_out_edge = [&](Struct *ptr, auto &ftor){
                bool ok = true;
                auto lambda = [&](auto &member){
                    auto it = name__struct_.find(member.type);
                    if (it != name__struct_.end())
                        ok = ok && ftor(&it->second);
                };
                ptr->each(lambda);
                return ok;
            };
            for (auto &p: name__struct_)
            {
                auto on_failure = [&](){
                    std::cout << "Input hierarchy is not a DAG: ";
                    gubg::OnlyOnce skip_arrow;
                    for (auto ptr: order_->cycle)
                    {
                        if (!skip_arrow())
                            std::cout << " => ";
                        std::cout << ptr->name;
                    }
                    std::cout << std::endl;
                };
                MSS(order_->process(&p.second, each_out_edge, false), on_failure());
            }

            MSS_END();
        }

        std::map<std::string, Struct> name__struct_;

        using Order = graph::TopologicalOrder<Struct *>;
        std::optional<Order> order_;
    };

} } 

#endif
