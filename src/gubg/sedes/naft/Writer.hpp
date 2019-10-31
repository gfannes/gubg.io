#ifndef HEADER_gubg_sedes_naft_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_naft_Writer_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>
#include <ostream>

namespace gubg { namespace sedes { namespace naft { 

    class Writer
    {
    public:
        Writer(std::ostream &os): os_(os) { }

        template <typename Obj>
        bool enter(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            indent_(level_++) << "[" << name << ":" << type << "]{" << std::endl;
            MSS_END();
        }
        template <typename Obj>
        bool exit(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            indent_(--level_) << "}" << std::endl;
            MSS_END();
        }

        template <typename Obj>
        bool leaf(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            indent_() << "[" << name << ":" << type << "](value:";
            stream_(obj);
            os_ << ")" << std::endl;
            MSS_END();
        }

        template <typename Obj>
        bool optional(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }

        template <typename Obj>
        bool array(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            if (oc)
                indent_(level_++) << "[" << name << ":" << type << "*](size:" << obj.size() << "){" << std::endl;
            else
                indent_(--level_) << "}" << std::endl;
            MSS_END();
        }

    private:
        std::ostream &indent_(unsigned int level) { return os_ << std::string(level*2, ' '); }
        std::ostream &indent_() { return indent_(level_); }
        std::ostream &os_;
        unsigned int level_ = 0;

        void stream_(char ch) {os_ << (int)ch;}
        template <typename T>
        void stream_(T v) {os_ << v;}
    };

} } } 

#endif
