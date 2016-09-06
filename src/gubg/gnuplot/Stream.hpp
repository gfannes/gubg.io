#ifndef HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <fstream>
#include <string>

namespace gubg { namespace gnuplot { 

    class Stream
    {
        public:
            Stream(const std::string &fn): fn_(fn), of_(fn_, std::ios::out)
            {
            }

            template <typename Array>
                bool add(const Array &ary)
                {
                    MSS_BEGIN(bool);
                    for (const auto &v: ary)
                    {
                        of_ << v << ' ';
                    }
                    of_ << std::endl;
                    MSS_END();
                }

        private:
            const std::string fn_;
            std::ofstream of_;
    };

} } 

#endif
