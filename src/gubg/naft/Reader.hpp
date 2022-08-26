#ifndef HEADER_gubg_naft_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Reader_hpp_ALREADY_INCLUDED

#include <gubg/Strange.hpp>

#include <string>
#include <optional>
#include <vector>

namespace gubg { namespace naft { 

    class Reader
    {
    public:
        struct Item
        {
            enum What {NodeOpen, NodeClose, Attribute, Text};
            What what;
            std::string text;
        };

        std::optional<std::string> error;

        Reader() {}
        Reader(const std::string &str) {setup(str);}

        void setup(const std::string &str);

        bool operator()(Item &);

    private:
        void pop_whitespace_();
        bool error_(const std::string &msg);

        enum State {Block, Attributes};
        friend std::ostream &operator<<(std::ostream &os, const Reader::State &);
        State state_ = State::Block;

        Strange strange_;
        std::vector<std::string> stack_;
    };

    std::ostream &operator<<(std::ostream &os, const Reader::Item::What &);
    std::ostream &operator<<(std::ostream &os, const Reader::State &);
    std::ostream &operator<<(std::ostream &os, const Reader::Item &);

} } 

#endif
