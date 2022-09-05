#ifndef HEADER_gubg_xml_writer_Prolog_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_writer_Prolog_hpp_ALREADY_INCLUDED

#include <gubg/parse/xml/Coding.hpp>

#include <ostream>

namespace gubg { namespace xml { namespace writer {

    class Prolog
    {
    public:
        Prolog(std::ostream &os): os_(&os) { *os_ << "<?xml"; }
        Prolog(Prolog &&dying): os_(dying.os_)
        {
            dying.os_ = nullptr;
        }
        ~Prolog()
        {
            if (!os_)
                return;
            *os_ << " ?>" << std::endl;
        }

            template <typename Key, typename Value>
        Prolog &attr(const Key &key, const Value &value)
        {
            *os_ << " " << encode(key) << "=" << "\"" << encode(value) << "\"";
            return *this;
        }
            template <typename Key, typename Value>
        Prolog &attr_raw(const Key &key, const Value &value)
        {
            *os_ << " " << key << "=" << "\"" << value << "\"";
            return *this;
        }

    private:
        std::ostream *os_ = nullptr;
    };

} } } 

#endif
