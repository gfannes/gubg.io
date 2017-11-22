#ifndef HEADER_gubg_naft_Document_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Document_hpp_ALREADY_INCLUDED

#include "gubg/naft/Node.hpp"
#include <ostream>

namespace gubg { namespace naft {

    class Document
    {
    public:
        Document(std::ostream &os): os_(os) {}
        ~Document()
        {
            os_ << std::endl;
        }

        Node node(const std::string &name)
        {
            Node n(0, os_, name);
            return n;
        }

    private:
        std::ostream &os_;
    };

} } 

#endif
