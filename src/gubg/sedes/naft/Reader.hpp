#ifndef HEADER_gubg_sedes_naft_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_naft_Reader_hpp_ALREADY_INCLUDED

#include <gubg/naft/Range.hpp>
#include <gubg/mss.hpp>
#include <list>
#include <cstring>

namespace gubg { namespace sedes { namespace naft { 

    class Reader
    {
    public:
        Reader(const std::string &content)
        {
            range_stack_.emplace_back(content);
        }

        template <typename Obj>
        bool enter(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);

            auto &range = range_stack_.back();

            std::string n,t;
            MSS(range.pop_tag(n,t));
            MSS(n == name);
            MSS(t == type);

            range_stack_.emplace_back();
            range.pop_block(range_stack_.back());

            std::memset(&obj, 0, sizeof(obj));

            MSS_END();
        }
        template <typename Obj>
        bool exit(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            range_stack_.pop_back();
            MSS_END();
        }

        template <typename Obj>
        bool leaf(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);

            auto &range = range_stack_.back();

            std::string n,t;
            MSS(range.pop_tag(n,t));
            MSS(n == name);
            MSS(t == type);

            const auto attrs = range.pop_attrs();
            auto it = attrs.find("value");
            MSS(it != attrs.end());
            assign_(obj, it->second);

            MSS_END();
        }

        template <typename Obj>
        bool optional(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);

            if (oc)
            {
                auto &range = range_stack_.back();
                auto sp = range.savepoint();

                std::string n,t;
                MSS(range.pop_tag(n,t));
                if (n == name)
                {
                    MSS(t == type);
                    obj.emplace();
                }
            }

            MSS_END();
        }

        template <typename Obj>
        bool array(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);

            if (oc)
            {
                auto &range = range_stack_.back();

                std::string n,t;
                MSS(range.pop_tag(n,t));
                MSS(n == name);
                MSS(t == type+"*");

                range_stack_.emplace_back();
                const auto attrs = range.pop_attrs();
                auto it = attrs.find("size");
                MSS(it != attrs.end());
                obj.resize(std::stoul(it->second));

                range.pop_block(range_stack_.back());
            }
            else
            {
                range_stack_.pop_back();
            }

            MSS_END();
        }

    private:
        void assign_(float &dst, const std::string &src) { dst = std::stof(src); }
        void assign_(int &dst, const std::string &src) { dst = std::stoi(src); }
        void assign_(char &dst, const std::string &src) { dst = std::stoi(src); }
        void assign_(bool &dst, const std::string &src) { dst = std::stoi(src) == 1; }

        std::list<gubg::naft::Range> range_stack_;
    };

} } } 

#endif
