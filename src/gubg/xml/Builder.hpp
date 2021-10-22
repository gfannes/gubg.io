#ifndef HEADER_gubg_xml_Builder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Builder_hpp_ALREADY_INCLUDED

#include "gubg/parse/xml/Coding.hpp"
#include <ostream>

namespace gubg { namespace xml { namespace builder {

    enum CloseType {ShortClose, NoShortClose};
    class Tag
    {
        public:
            Tag(std::ostream &os, const std::string &name, CloseType close_type = ShortClose, const size_t indent = 0)
                :os_(os), name_(name), indent_(indent), state_(Attributes), close_type_(close_type)
            {
                if (indent_)
                    os_ << std::string(2*indent_, ' ');
                os_ << "<" << name_;
            }
            Tag(Tag &&rhs):
                os_(rhs.os_),
                name_(std::move(rhs.name_)),
                indent_(rhs.indent_),
                state_(rhs.state_),
                did_stream_(rhs.did_stream_),
                close_type_(rhs.close_type_)
        {
            rhs.state_ = State::Dead;
        }
            template <typename Name>
                Tag &attr(const Name &name, const std::string &value)
                {
                    os_ << " " << encode(name) << "=" << "\"" << encode(value) << "\"";
                    return *this;
                }
            template <typename Name, typename Value>
                Tag &attr(const Name &name, const Value &value)
                {
                    os_ << " " << encode(name) << "=" << "\"" << value << "\"";
                    return *this;
                }
            template <typename Name, typename Value>
                Tag &attr_raw(const Name &name, const Value &value)
                {
                    os_ << " " << name << "=" << "\"" << value << "\"";
                    return *this;
                }
            Tag tag(const std::string &name)
            {
                close_attributes_();
                os_ << std::endl;
                return Tag(os_, name, close_type_, indent_+1);
            }
            template <typename T>
                Tag &operator<<(const T &t)
                {
                    close_attributes_();
                    os_ << t;
                    did_stream_ = true;
                    return *this;
                }
            ~Tag()
            {
                switch (state_)
                {
                    case Attributes:
                        switch (close_type_)
                        {
                            case ShortClose:
                                os_ << "/>";
                                break;
                            case NoShortClose:
                                os_ << "></" << name_ << ">";
                                break;
                        }
                        break;
                    case Elements:
                        if (!did_stream_)
                            os_ << std::endl << std::string(2*indent_, ' ');
                        os_ << "</" << name_ << ">";
                        break;
                    default: break;
                }
            }

        private:
            Tag(const Tag &);

            void close_attributes_()
            {
                if (state_ != Attributes)
                    return;
                os_ << ">";
                state_ = Elements;
            }

            std::ostream &os_;
            const std::string name_;
            const size_t indent_;
            enum State {Attributes, Elements, Closed, Dead};
            State state_;
            bool did_stream_ = false;
            CloseType close_type_;
    };
    class Header
    {
        public:
            Header(std::ostream &os): os_(os) { os_ << "<?xml"; }
            ~Header() { os_ << "?>" << std::endl; }
            template <typename Name, typename Value>
                Header &attr(const Name &name, const Value &value)
                {
                    os_ << " " << encode(name) << "=" << "\"" << encode(value) << "\"";
                    return *this;
                }
            template <typename Name, typename Value>
                Header &attr_raw(const Name &name, const Value &value)
                {
                    os_ << " " << name << "=" << "\"" << value << "\"";
                    return *this;
                }
        private:
            std::ostream &os_;
    };
    class Comment
    {
        public:
            Comment(std::ostream &os): os_(os) { os_ << "<!--"; }
            ~Comment() { os_ << "-->" << std::endl; }
            template <typename T>
                Comment &operator<<(const T &t) {os_ << t; return *this;}
        private:
            std::ostream &os_;
    };

} } } 

#endif
