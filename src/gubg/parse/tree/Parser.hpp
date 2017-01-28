#ifndef HEADER_gubg_parse_tree_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_tree_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/tree/Codes.hpp"
#include "gubg/Strange.hpp"
#include "gubg/mss.hpp"
#include <string>
#include <map>

namespace gubg { namespace parse { namespace tree { 

    using Name = std::string;
    using Attributes = std::map<std::string, std::string>;

    template <typename Receiver>
        class Parser_crtp
        {
            private:
                constexpr static const char *logns = nullptr;//"tree::Parser";

            public:
                ReturnCode process(const std::string &content)
                {
                    MSS_BEGIN(ReturnCode, logns);
                    content_ = gubg::Strange(content);
                    const auto rc = process_();
                    content_.clear();
                    MSS(rc);
                    MSS_END();
                }

            private:
                ReturnCode process_()
                {
                    MSS_BEGIN(ReturnCode, logns);

                    while (true)
                    {
                        Name name;
                        switch (const auto rc = pop_name_(name))
                        {
                            case ReturnCode::OK:
                                break;
                            case ReturnCode::NotFound:
                                MSS_RETURN_OK();
                                break;
                            default: MSS(rc); break;
                        }

                        Attributes attrs;
                        MSS(pop_attrs_(attrs));

                        receiver_().parser_open(name, attrs);

                        pop_whitespace_();
                        if (content_.pop_if('{'))
                        {
                            pop_whitespace_();
                            while (!content_.pop_if('}'))
                            {
                                MSS(process_());
                            }
                        }

                        receiver_().parser_close();
                    }

                    MSS_END();
                }
                static void pop_whitespace_(gubg::Strange &strange)
                {
                    strange.strip(" \t\n\r");
                }
                void pop_whitespace_()
                {
                    pop_whitespace_(content_);
                }
                ReturnCode pop_name_(Name &name)
                {
                    MSS_BEGIN(ReturnCode, logns);
                    pop_whitespace_();
                    if (content_.empty() || content_.front() != '[')
                    {
                        MSS_Q(ReturnCode::NotFound);
                    }
                    MSS(content_.pop_bracket(name, "[]"));
                    L(C(name));
                    MSS_END();
                }
                ReturnCode pop_attrs_(Attributes &attrs)
                {
                    MSS_BEGIN(ReturnCode, logns);
                    gubg::Strange attr;
                    while ((pop_whitespace_(), content_.pop_bracket(attr, "()")))
                    {
                        std::string key, value;
                        pop_whitespace_(attr);
                        MSS(attr.pop_until(key, ':'));
                        MSS(attr.pop_all(value));
                        L(C(key)C(value));
                        attrs[key] = value;
                    }
                    MSS_END();
                }
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                gubg::Strange content_;
                gubg::Strange dump_;
        };

} } } 

#endif
