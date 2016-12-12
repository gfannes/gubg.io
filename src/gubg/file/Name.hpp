#ifndef HEADER_gubg_file_Name_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Name_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>

namespace gubg { namespace file { 

    class Name
    {
        public:
            Name() = default;
            Name(const std::string &name): name_(name) {}
            Name(const char *name): name_(name) {}

            operator const std::string & () const
            {
                return name_;
            }

            void stream(std::ostream &os) const
            {
                os << name_;
            }

        private:
            std::string name_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Name &name)
    {
        name.stream(os);
        return os;
    }

} } 

#endif
