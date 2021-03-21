#include <gubg/wav/Writer.hpp>

namespace gubg { namespace wav { 

    bool Writer::open(const std::string &filename, unsigned int block_size, unsigned int channel_count, unsigned int sample_rate, unsigned int bit_depth)
    {
        MSS_BEGIN(bool);

        //Make sure previuous sizes are written correctly
        close();

        fo_.open(filename, std::ios::binary | std::ios::out);
        MSS(fo_.is_open(), std::cout << "Error: could not open \"" << filename << "\" for writing WAVE" << std::endl);

        block_size_ = block_size;
        channel_count_ = channel_count;
        sample_rate_ = sample_rate;
        bit_depth_ = bit_depth;

        {
            fo_ << "RIFF";
            riff_size_pos_ = fo_.tellp();
            fo_ << "????";
            fo_ << "WAVE";
        }

        {
            fo_ << "fmt ";
            const auto size_pos = fo_.tellp();
            fo_ << "????";
            stream16_(1);
            stream16_(channel_count_);
            stream32_(sample_rate_);
            const unsigned int block_align = (channel_count_*bit_depth_)/8;
            byte_buffer_.resize(block_align*block_size_);
            stream32_(sample_rate_*block_align);
            stream16_(block_align);
            stream16_(bit_depth_);

            stream_size_(size_pos);
        }

        {
            fo_ << "data";
            data_size_pos_ = fo_.tellp();
            fo_ << "????";
        }

        MSS_END();
    }

    void Writer::close()
    {
        if (fo_.is_open())
        {
            stream_size_(data_size_pos_);
            stream_size_(riff_size_pos_);
            fo_.close();
        }
    }

    unsigned int Writer::block_size() const
    {
        return block_size_;
    }

    bool Writer::write_mono(const float *src)
    {
        MSS_BEGIN(bool);

        MSS(valid());

        MSS(channel_count_ == 1, std::cout << "Error: only mono files are supported for write_mono()" << std::endl);

        MSS(setup_mono_(src, 0u));

        fo_.write((const char *)byte_buffer_.data(), byte_buffer_.size());

        MSS_END();
    }

    //Privates
    bool Writer::setup_mono_(const float *src, unsigned int chix)
    {
        MSS_BEGIN(bool);

        MSS(!!src);

        const auto bytes_per_sample = bit_depth_/8;
        const auto block_align = channel_count_*bytes_per_sample;

        auto dst = byte_buffer_.begin()+chix*bytes_per_sample;
        switch (bit_depth_)
        {
            case 16:
                for (auto six = 0u; six < block_size_; ++six, dst += bytes_per_sample)
                {
                    std::int32_t v = src[six];
                    dst[0] = v;
                    v >>= 8; dst[1] = v;
                }
                break;
            case 24:
                for (auto six = 0u; six < block_size_; ++six, dst += bytes_per_sample)
                {
                    std::int32_t v = src[six];
                    dst[0] = v;
                    v >>= 8; dst[1] = v;
                    v >>= 8; dst[2] = v;
                }
                break;
            default:
                MSS(false, std::cout << "Error: unsupported bit depth " << bit_depth_ << std::endl);
                break;
        }
        MSS_END();
    }

} } 
