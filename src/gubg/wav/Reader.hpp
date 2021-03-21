#ifndef HEADER_gubg_wav_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Reader_hpp_ALREADY_INCLUDED

#include <gubg/wav/Format.hpp>
#include <gubg/file/Range.hpp>
#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace gubg { namespace wav { 

    class Reader
    {
    public:
        std::optional<Format> format;

        bool valid() const {return !!format;}

        bool load(const std::string &filepath, unsigned int block_size);

        unsigned int block_size() const;
        unsigned int block_count() const;

        bool read_block(unsigned int block_ix, float *dst);

    private:
        std::ifstream fi_;
        gubg::file::Range data_;
        unsigned int block_size_{};
        unsigned int block_count_{};
        std::vector<std::uint8_t> block_buffer_;
    };

} } 

#endif
