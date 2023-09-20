#ifndef HEADER_gubg_file_Range_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Range_hpp_ALREADY_INCLUDED

#include <fstream>
#include <array>
#include <cassert>
#include <cstdint>

namespace gubg { namespace file { 

    class Range
    {
    public:
        Range() {}
        Range(std::ifstream &fi): fi_(&fi) {}

        void clear() {*this = Range();}

        //Reloads the file stream and starts from its begin again
        bool reload();

        bool valid() const {return !!fi_;}
        std::size_t size() const {return byte_count_ - ix_;}
        bool empty() const {return size() == 0;}

        bool pop_count(unsigned int count)
        {
            if (!valid())
                return false;
            if (size() < count)
                return false;
            ix_ += count;
            return true;
        }

        template <typename Byte>
        bool pop_raw(Byte *dst, std::size_t count)
        {
            if (!valid())
                return false;
            if (size() < count)
                return false;
            static_assert(sizeof(*dst) == 1, "Range.pop_raw() only works on bytes");
            fi_->seekg(ix_);
            fi_->read((char*)dst, count);
            ix_ += count;
            return true;
        }

        bool pop(Range &dst, std::size_t count)
        {
            if (!valid())
                return false;
            if (size() < count)
                return false;
            dst.fi_ = fi_;
            dst.ix_ = ix_;
            dst.byte_count_ = dst.ix_+count;
            ix_ += count;
            return true;
        }

        template <typename Container>
        bool pop(Container &dst) { return pop_raw(dst.data(), dst.size()); }

        bool pop_le(std::uint32_t &v)
        {
            std::array<std::uint8_t, 4> bytes;
            if (!pop(bytes)) return false;
            v = 0;   v |= bytes[3];
            v <<= 8; v |= bytes[2];
            v <<= 8; v |= bytes[1];
            v <<= 8; v |= bytes[0];
            return true;
        }
        bool pop_le(std::uint16_t &v)
        {
            std::array<std::uint8_t, 2> bytes;
            if (!pop(bytes)) return false;
            v = 0;   v |= bytes[1];
            v <<= 8; v |= bytes[0];
            return true;
        }

    private:
        std::ifstream *fi_ = nullptr;
        std::size_t ix_ = {};
        std::size_t byte_count_{};
    };

} } 

#endif
