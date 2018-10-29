#ifndef HEADER_gubg_t2_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Parser_hpp_ALREADY_INCLUDED

#include "gubg/t2/Types.hpp"
#include "gubg/mss.hpp"
#include <cstdint>

namespace gubg { namespace t2 { 

    enum class State
    {
        Idle, Start, Tag, Key, Value,
    };

    template <typename Receiver>
    class Parser_crtp
    {
    public:
        bool process(const Byte byte)
        {
            MSS_BEGIN(bool, "");
            L(C(byte, int));
            switch (byte & mask_md)
            {
                case md_special:
                    L("Special");
                    switch (byte)
                    {
                        case md_som: change_state_(State::Start); break;
                        case md_eom: change_state_(State::Idle); break;
                        case md_close_block:
                            break;
                        case md_empty_tag:
                            break;
                        case md_empty_attr:
                            break;
                    }
                    break;
                case md_open_tag:
                    L("Open tag");
                    change_state_(State::Tag);
                    break;
                case md_open_attr:
                    L("Open attr");
                    break;
                case md_data:
                    L("Data");
                    break;
            }

            const std::uint32_t data = (byte & mask_data);
            switch (state_)
            {
                case State::Tag:
                    tag_ <<= 6; tag_ |= data;
                    break;
                case State::Key:
                    key_ <<= 6; key_ |= data;
                    break;
                case State::Value:
                    value_ <<= 6; value_ |= data;
                    break;
            }

            MSS_END();
        }
    private:
        Receiver &receiver_(){return *static_cast<Receiver *>(this);}

        void change_state_(State wanted_state)
        {
            //Exit
            switch (state_)
            {
                case State::Idle:
                    receiver_().t2_eom();
                    break;
                case State::Start:
                    break;
                case State::Tag:
                    receiver_().t2_tag(&tag_);
                    break;
                case State::Key:
                    break;
                case State::Value:
                    receiver_().t2_attr(&key_, &value_);
                    break;
            }

            state_ = wanted_state;

            //Enter
            switch (state_)
            {
                case State::Idle:
                    break;
                case State::Start:
                    receiver_().t2_som();
                    break;
                case State::Tag:
                    tag_ = 0;
                    break;
                case State::Key:
                    key_ = 0;
                    break;
                case State::Value:
                    value_ = 0;
                    break;
            }
        }

        Data tag_, key_, value_;
        State state_ = State::Idle;
    };

} } 

#endif
