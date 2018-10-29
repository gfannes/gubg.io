#ifndef HEADER_gubg_t2_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Parser_hpp_ALREADY_INCLUDED

#include "gubg/t2/Types.hpp"
#include "gubg/mss.hpp"
#include <cstdint>
#include <ostream>

namespace gubg { namespace t2 { 

    enum class State
    {
        Idle,
        Open,    //Block is open at level
        Tag, //Reading a block tag
        Key,     //Reading an attribute key
        Value,   //Reading an attribute value
        Closing, //Closing a block
    };
    std::ostream &operator<<(std::ostream &os, State state)
    {
        switch (state)
        {
#define L_CASE(name) case State::name: os << #name; break
            L_CASE(Idle);
            L_CASE(Open);
            L_CASE(Tag);
            L_CASE(Key);
            L_CASE(Value);
            L_CASE(Closing);
            default: os << "Unknown state"; break;
        }
        return os;
    }

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
                        case md_som:
                            change_state_(State::Idle);
                            change_state_(State::Open);
                            break;
                        case md_eom:
                            change_state_(State::Idle);
                            break;
                        case md_close_block:
                            change_state_(State::Closing);
                            break;
                    }
                    break;
                case md_open_tag:
                    L("Reading tag");
                    change_state_(State::Tag);
                    break;
                case md_open_attr:
                    L("Reading attr [key|value]");
                    //If we were parsing the key, we continue with value, else with key
                    change_state_(state_ == State::Key ? State::Value : State::Key);
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
            S("");
            L(C(state_)C(wanted_state));

            if (wanted_state == state_)
                return;

            L("Leaving state " << state_);
            switch (state_)
            {
                case State::Idle:
                    level_ = 0;
                    break;
                case State::Open:
                    break;
                case State::Tag:
                    receiver_().t2_open(tag_, level_);
                    break;
                case State::Key:
                    break;
                case State::Value:
                    receiver_().t2_attr(key_, value_);
                    break;
                case State::Closing:
                    break;
            }

            state_ = wanted_state;

            L("Entering state " << state_);
            switch (state_)
            {
                case State::Idle:
                    while (level_ > 0)
                    {
                        receiver_().t2_close(level_);
                        --level_;
                    }
                    receiver_().t2_close(level_);
                    break;
                case State::Open:
                    if (level_ == 0)
                        receiver_().t2_open(0, level_);
                    break;
                case State::Tag:
                    ++level_;
                    tag_ = 0;
                    break;
                case State::Key:
                    key_ = 0;
                    break;
                case State::Value:
                    value_ = 0;
                    break;
                case State::Closing:
                    receiver_().t2_close(level_);
                    if (level_ == 0)
                    {
                        change_state_(State::Idle);
                    }
                    else
                    {
                        --level_;
                        change_state_(State::Open);
                    }
                    break;
            }
        }

        Data tag_, key_, value_;
        unsigned int level_;
        State state_ = State::Idle;
    };

} } 

#endif
