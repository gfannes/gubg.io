#ifndef HEADER_gubg_frame_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_frame_Decoder_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>
#include <vector>

namespace gubg { namespace frame { 

    template <typename Receiver>
    class Decoder_crtp
    {
    public:
        Decoder_crtp(std::uint8_t soh): soh_(soh) { }

        //CRTP event points
        //* Mandatory
        //void on_payload(std::uint64_t type, std::vector<std::uint8_t> &payload)
        //* Optional
        void on_soh(){}
        void on_type(std::uint64_t){}
        void on_size(std::uint64_t){}
        //* Errors
        void error_broken_frame(){}
        void error_expected_soh(std::uint8_t){}
        void error_missing_msb(std::uint8_t){}

        bool process(std::uint8_t byte)
        {
            MSS_BEGIN(bool);

            if (byte == soh_)
            {
                const bool was_expecting_soh = (state_ == WaitingForSOH);
                if (!was_expecting_soh)
                    receiver_().error_broken_frame();
                change_state_(ReceivingType);
                receiver_().on_soh();
                MSS(was_expecting_soh);
            }
            else
                switch (state_)
                {
                    case WaitingForSOH:
                        MSS(false, receiver_().error_expected_soh(byte));
                        break;

                    case ReceivingType:
                        {
                            byte ^= soh_;
                            const bool has_msb_set = (byte & 0x80);
                            MSS(has_msb_set, receiver_().error_missing_msb(byte));
                            type_ <<= 6;
                            type_ |= (byte & 0x3f);
                            const bool is_last = (byte & 0x40);
                            if (is_last)
                            {
                                receiver_().on_type(type_);
                                change_state_(ReceivingSize);
                            }
                        }
                        break;

                    case ReceivingSize:
                        {
                            byte ^= soh_;
                            const bool has_msb_set = (byte & 0x80);
                            MSS(has_msb_set, receiver_().error_missing_msb(byte));
                            size_ <<= 6;
                            size_ |= (byte & 0x3f);
                            const bool is_last = (byte & 0x40);
                            if (is_last)
                            {
                                receiver_().on_size(size_);
                                change_state_(ReceivingFlips);
                            }
                        }
                        break;

                    case ReceivingFlips:
                        {
                            byte ^= soh_;
                            const std::uint8_t value = (byte & 0x7f);
                            if (byte & 0x80)
                                flips_[ix_++] = value;
                            else
                                ix_ += value;
                            if (ix_ >= flips_.size())
                                change_state_(ReceivingPayload);
                        }
                        break;

                    case ReceivingPayload:
                        {
                            payload_[ix_++] ^= byte;
                            if (ix_ == payload_.size())
                                change_state_(ReceivedPayload);
                        }
                        break;
                }

            MSS_END();
        }

    private:
        enum State {WaitingForSOH, ReceivingType, ReceivingSize, ReceivingFlips, ReceivingPayload, ReceivedPayload};

        Receiver &receiver_() {return *static_cast<Receiver*>(this);}

        void change_state_(State new_state)
        {
            if (new_state == state_)
                return;

            //Exit
            switch (state_)
            {
            }

            state_ = new_state;

            //Enter
            switch (state_)
            {
                case ReceivingType:
                    type_ = 0;
                    break;
                case ReceivingSize:
                    size_ = 0;
                    flips_.resize(0);
                    break;
                case ReceivingFlips:
                    flips_.resize((size_+6)/7);
                    ix_ = 0;
                    break;
                case ReceivingPayload:
                    //Setup the payload_ with 0x00 or 0x01, depending on whether the corresponding data byte is flipped or not
                    //When receiving the actual data, it will be xor-ed on the 0x00/0x01
                    //To minimize book-keeping, we first do as if we are working of multiples of 7: each flip byte corresponds with 7 payload bytes
                    payload_.resize(flips_.size()*7);
                    ix_ = 0;
                    for (auto flip: flips_)
                    {
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                        payload_[ix_++] = (flip & 0x01); flip >>= 1;
                    }

                    //Set the payload size correctly
                    assert(size_ <= payload_.size());
                    payload_.resize(size_);
                    ix_ = 0;

                    if (ix_ == payload_.size())
                        change_state_(ReceivedPayload);
                    break;
                case ReceivedPayload:
                    receiver_().on_payload(type_, payload_);
                    change_state_(WaitingForSOH);
                    break;
            }
        }

        const std::uint8_t soh_;
        State state_{WaitingForSOH};

        std::size_t ix_;

        std::uint64_t type_;
        std::uint64_t size_;
        std::vector<std::uint8_t> flips_;
        std::vector<std::uint8_t> payload_;
    };

} } 

#endif
