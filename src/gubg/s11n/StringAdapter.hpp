#ifndef HEADER_gubg_s11n_StringAdapter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_StringAdapter_hpp_ALREADY_INCLUDED

namespace gubg { namespace s11n { 

    template <typename String>
    struct StringAdapter
    {
        String &str_;
        StringAdapter(String &str): str_(str) {}

        bool push_back(char ch)
        {
            return str_.push_back(ch);
        }
    };

    template <>
    struct StringAdapter<std::string>
    {
        std::string &str_;
        StringAdapter(std::string &str): str_(str) {}

        bool push_back(char ch)
        {
            str_.push_back(ch);
            return true;
        }
    };

} } 

#endif
