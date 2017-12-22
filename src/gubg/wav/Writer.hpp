#ifndef HEADER_gubg_wav_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Writer_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <string>
#include <fstream>

namespace gubg { namespace wav { 

    class Writer
    {
    public:
        Writer(const std::string &filename, unsigned int nr_channels, unsigned int samplerate): fn_(filename), nr_channels_(nr_channels), samplerate_(samplerate), of_(filename, std::ios::binary | std::ios::out)
        {
            {
                of_ << "RIFF";
                riff_size_pos_ = of_.tellp();
                of_ << "????";
                of_ << "WAVE";
            }

            {
                of_ << "fmt ";
                const auto size_pos = of_.tellp();
                of_ << "????";
                stream16_(1);
                stream16_(nr_channels_);
                stream32_(samplerate_);
                const unsigned int bits_per_sample = 24;
                const unsigned int block_align = (nr_channels_*bits_per_sample)/8;
                stream32_(samplerate_*block_align);
                stream16_(block_align);
                stream16_(bits_per_sample);

                stream_size_(size_pos);
            }

            {
                of_ << "data";
                data_size_pos_ = of_.tellp();
                of_ << "????";
            }
        }
        ~Writer()
        {
            stream_size_(data_size_pos_);
            stream_size_(riff_size_pos_);
        }

        template <typename Sample>
        bool add_sample(const Sample &sample)
        {
            MSS_BEGIN(bool);
            MSS(sample.size() == nr_channels_);
            for (auto v: sample)
                stream_value_(v);
            MSS_END();
        }

    private:
        void stream16_(unsigned int v)
        {
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
        }
        void stream24_(unsigned int v)
        {
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
        }
        void stream32_(unsigned int v)
        {
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
            of_.put(v & 0xff); v >>= 8;
        }
        void stream_size_(std::ofstream::pos_type pos)
        {
            const auto current_pos = of_.tellp();
            of_.seekp(pos);
            stream32_(current_pos-pos-4);
            of_.seekp(current_pos);
        }
        void stream_value_(float v)
        {
            const int factor = (1<<23)-1;
            const int vv = v*factor;
            stream24_(vv);
        }

        const std::string fn_;
        const unsigned int nr_channels_;
        const unsigned int samplerate_;
        std::ofstream of_;
        std::ofstream::pos_type riff_size_pos_;
        std::ofstream::pos_type data_size_pos_;
    };

} } 

#endif
