#ifndef HEADER_gubg_wav_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Writer_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <string>
#include <fstream>
#include <array>
#include <vector>
#include <cstdint>

namespace gubg { namespace wav { 

    class Writer
    {
    public:
        ~Writer() {close();}

        bool valid() const {return fo_.is_open();}

        bool open(const std::string &filename, unsigned int block_size, unsigned int channel_count, unsigned int sample_rate, unsigned int bit_depth = 24);
        void close();

        bool write_mono(const float *src);

        template <typename Ftor>
        bool write_block(Ftor &&ftor)
        {
            MSS_BEGIN(bool);
            MSS(valid());
            for (auto chix = 0u; chix < channel_count_; ++chix)
            {
                const float *ptr = ftor(chix);
                MSS(setup_mono_(ptr, chix));
            }
            fo_.write((const char *)byte_buffer_.data(), byte_buffer_.size());
            MSS_END();
        }

        template <typename Sample>
        bool add_sample(const Sample &sample)
        {
            MSS_BEGIN(bool);
            MSS(block_size_ == 1);
            float flt;
            auto lambda = [&](auto chix){
                flt = sample[chix];
                return &flt;
            };
            MSS(write_block(lambda));
            MSS_END();
        }

    private:
        bool setup_mono_(const float *src, unsigned int chix);

        void stream16_(unsigned int v)
        {
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
        }
        void stream24_(unsigned int v)
        {
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
        }
        void stream32_(unsigned int v)
        {
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
            fo_.put(v & 0xff); v >>= 8;
        }
        void stream_size_(std::ofstream::pos_type pos)
        {
            const auto current_pos = fo_.tellp();
            fo_.seekp(pos);
            stream32_(current_pos-pos-4);
            fo_.seekp(current_pos);
        }
        void stream_value_(float v)
        {
            const int factor = (1<<23)-1;
            const int vv = v*factor;
            stream24_(vv);
        }

        std::ofstream fo_;
        unsigned int sample_rate_ = 0;
        unsigned int channel_count_ = 0;
        unsigned int block_size_ = 0;
        unsigned int bit_depth_ = 0;
        std::ofstream::pos_type riff_size_pos_;
        std::ofstream::pos_type data_size_pos_;
        std::vector<std::uint8_t> byte_buffer_;
    };

} } 

#endif
