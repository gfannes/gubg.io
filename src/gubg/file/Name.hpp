#ifndef HEADER_gubg_file_Name_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Name_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include <string>
#include <ostream>

namespace gubg { namespace file { 

    class Name
    {
        public:
            Name() = default;
            Name(const std::filesystem::path &name): name_(name.string()) {}
            Name(const std::string &name): name_(name) {}
            Name(const char *name): name_(name) {}

            operator const std::string & () const { return name_; }
            const std::string &str() const {return name_;}

            bool operator<(const Name &rhs)  const { return name_ <  rhs.name_; }
            bool operator>(const Name &rhs)  const { return name_ >  rhs.name_; }
            bool operator==(const Name &rhs) const { return name_ == rhs.name_; }
            bool operator!=(const Name &rhs) const { return name_ != rhs.name_; }

            Name operator+(const std::string &part) const
            {
                Name res(name_);
                res.name_ += part;
                return res;
            }

            Name &operator/=(const std::string &part)
            {
                if (name_.empty())
                {
                    name_ = part;
                }
                else if (part.empty())
                {
                    //Nothing todo
                }
                else
                {
                    const bool name_has_slash = (name_.back() == '/');
                    const bool part_has_slash = (part.front() == '/');
                    if (name_has_slash)
                    {
                        if (part_has_slash)
                            name_ += part.substr(1);
                        else
                            name_ += part;
                    }
                    else
                    {
                        if (part_has_slash)
                            name_ += part;
                        else
                        {
                            name_.push_back('/');
                            name_ += part;
                        }
                    }
                }
                return *this;
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
