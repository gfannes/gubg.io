#ifndef HEADER_gubg_naft_Document_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Document_hpp_ALREADY_INCLUDED

#include <gubg/naft/Node.hpp>
#include <ostream>

namespace gubg { namespace naft {

    class Document: public Node
    {
    public:
        Document(std::ostream &os): Node(os) {}
        ~Document()
        {
            //We act as if we are dead, making sure ~Node will not close this node
            Node::os_ = nullptr;
        }
    };

} } 

#endif
