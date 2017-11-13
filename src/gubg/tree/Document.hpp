#ifndef HEADER_gubg_tree_Document_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Document_hpp_ALREADY_INCLUDED

#include "gubg/tree/Node.hpp"
#include <ostream>

namespace gubg { namespace tree { 

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
