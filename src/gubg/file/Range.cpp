#include <gubg/file/Range.hpp>

#include <gubg/mss.hpp>

namespace gubg { namespace file { 

    bool Range::reload()
    {
        MSS_BEGIN(bool);

        MSS(valid());
        MSS(fi_->is_open());

        fi_->seekg(0, std::ios::end);
        const auto end_pos = fi_->tellg();
        fi_->seekg(0, std::ios::beg);
        const auto beg_pos = fi_->tellg();
        byte_count_ = end_pos-beg_pos;

        MSS_END();
    }

} } 
