#ifndef HEADER_gubg_sedes_dsl_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_dsl_Parser_hpp_ALREADY_INCLUDED

#include <gubg/sedes/Catalogue.hpp>
#include <gubg/sedes/Struct.hpp>
#include <gubg/sedes/Member.hpp>
#include <gubg/naft/Range.hpp>
#include <gubg/mss.hpp>
#include <string>

namespace gubg { namespace sedes { namespace dsl { 

    class Parser
    {
    public:
        Parser(Catalogue &catalogue): catalogue_(catalogue) {}

        //Parses content into catalogue_
        bool process(const std::string &content)
        {
            MSS_BEGIN(bool);

            gubg::naft::Range range0{content};
            for (std::string name0,type0; range0.pop_tag(name0,type0); )
            {
                const auto attrs0 = range0.pop_attrs();

                MSS(type0 == "struct");
                Struct struc{name0};

                gubg::naft::Range range1; 
                if (range0.pop_block(range1))
                {
                    for (std::string name1,type1; range1.pop_tag(name1,type1); )
                    {
                        const auto attrs1 = range1.pop_attrs();

                        auto set_common_info = [&](Member &member){
                            member.name = name1;
                            {
                                auto it = attrs1.find("type");
                                if (it != attrs1.end())
                                    member.type = it->second;
                            }
                        };
                        Member::Ptr member_ptr;
                        if (false) {}
                        else if (type1 == "field")
                        {
                            member_ptr = Field::create([&](auto &f){set_common_info(f);});
                        }
                        else if (type1 == "optional")
                        {
                            member_ptr = Optional::create([&](auto &o){set_common_info(o);});
                        }
                        else if (type1 == "array")
                        {
                            member_ptr = Array::create([&](auto &a){set_common_info(a);});
                        }
                        struc.add(member_ptr);
                    }
                }

                MSS(catalogue_.add(struc));
            }

            MSS_END();
        }

    private:
        Catalogue &catalogue_;
    };

} } } 

#endif
