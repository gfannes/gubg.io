#include <gubg/wav/Reader.hpp>
#include <gubg/mss.hpp>
#include <array>
#include <cstdint>
#include <fstream>

namespace gubg { namespace wav { 

    using FourBytes = std::array<std::uint8_t, 4>;
    const FourBytes riff{'R','I','F','F'};
    const FourBytes wave{'W','A','V','E'};
    const FourBytes fmt_{'f','m','t',' '};
    const FourBytes data{'d','a','t','a'};

    bool Reader::load(const std::string &filepath, unsigned int block_size)
    {
        MSS_BEGIN(bool);

        MSS(block_size > 0, std::cout << "Error: block_size cannot be zero" << std::endl);

        data_.clear();
        block_count_ = 0;

        fi_.open(filepath, std::ios::binary);
        MSS(fi_.is_open(), std::cout << "Error: could not open WAVE file \"" << filepath << "\"" << std::endl);
        gubg::file::Range range{fi_};
        MSS(range.reload());

        FourBytes type;
        std::uint32_t size;
        MSS(range.pop(type) && type == riff);
        MSS(range.pop_le(size));
        MSS(range.pop(type) && type == wave);

        for (gubg::file::Range subchunk; range.pop(type) && range.pop_le(size) && range.pop(subchunk, size); )
        {
            L("Found subchunk " << type[0]<<type[1]<<type[2]<<type[3]);
            if (type == fmt_)
            {
                format.emplace();
                MSS(subchunk.pop_le(format->format));
                MSS(subchunk.pop_le(format->channel_count));
                MSS(subchunk.pop_le(format->sample_rate));
                MSS(subchunk.pop_le(format->byte_rate));
                MSS(subchunk.pop_le(format->block_align));
                MSS(subchunk.pop_le(format->bits_per_sample));
                MSS(subchunk.empty());
            }
            else if (type == data)
            {
                MSS(!!format);
                data_ = subchunk;
                L("File contains " << data_.size()/format->block_align << " audio samples");
                break;
            }
            else
            {
                //Skip unknown subchunks
            }
        }

        MSS(!!format);
        MSS(data_.valid());

        block_size_ = block_size;
        block_buffer_.resize(block_size_*format->block_align);
        block_count_ = data_.size()/block_buffer_.size();

        MSS_END();
    }

    unsigned int Reader::block_size() const
    {
        return block_size_;
    }
    unsigned int Reader::block_count() const
    {
        return block_count_;
    }

    bool Reader::read_block(unsigned int block_ix, float *dst, float factor)
    {
        MSS_BEGIN(bool);

        MSS(!!format);
        MSS(format->channel_count == 1, std::cout << "Error: only mono WAVE files are currently supported" << std::endl);

        MSS(block_ix < block_count());
        MSS(!!dst);

        gubg::file::Range range = data_;
        MSS(range.pop_count(block_ix*block_buffer_.size()));

        MSS(range.pop(block_buffer_));

        MSS(format->format == 1, std::cout << "Error: only PCM is supported" << std::endl);

        const auto sign_extension_bit_count = 32-format->bits_per_sample;

        const std::uint8_t *src = block_buffer_.data();
        switch (format->bits_per_sample)
        {
            case 16:
                for (auto six = 0u; six < block_size_; ++six, src += format->block_align)
                {
                    std::int32_t v = src[1];
                    v <<= 8; v |= src[0];
                    v <<= 16;
                    v >>= 16;
                    dst[six] = v*factor;
                }
                break;
            case 24:
                for (auto six = 0u; six < block_size_; ++six, src += format->block_align)
                {
                    std::int32_t v = src[2];
                    v <<= 8; v |= src[1];
                    v <<= 8; v |= src[0];
                    v <<= 8;
                    v >>= 8;
                    dst[six] = v*factor;
                }
                break;
            default:
                MSS(false, std::cout << "Error: unsupported bit depth " << format->bits_per_sample << std::endl);
                break;
        }

        MSS_END();
    }
} } 
