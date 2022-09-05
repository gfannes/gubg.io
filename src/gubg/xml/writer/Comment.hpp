#ifndef HEADER_gubg_xml_writer_Comment_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_writer_Comment_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace xml { namespace writer {

    class Comment
    {
        public:
            Comment(std::ostream &os): os_(&os) { *os_ << "<!--"; }
            Comment(Comment &&dying): os_(dying.os_)
            {
                dying.os_ = nullptr;
            }
            ~Comment()
            {
                if (!os_)
                    return;
                *os_ << "-->" << std::endl;
            }

            template <typename T>
            Comment &operator<<(const T &t)
            {
                *os_ << t;
                return *this;
            }

        private:
            std::ostream *os_ = nullptr;
    };

} } } 

#endif
