#ifndef HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include "gubg/mss.hpp"
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>

namespace gubg { namespace gnuplot { 

    class Stream
    {
        private:
            using Line = std::string;
            using Lines = std::list<Line>;
            struct Info
            {
                unsigned int col_count = 0;
                unsigned int max_nr_cols = 0;
                Lines lines;
                std::string name;
            };

        public:
            class Data
            {
                public:
                    Data(Info &info): info_(info)
                {
                    info_.col_count = 0;
                }
                    Data(Data &&dying): oss_(std::move(dying.oss_)), info_(dying.info_) {}
                    ~Data()
                    {
                        info_.lines.push_back(oss_.str());
                        info_.max_nr_cols = std::max(info_.max_nr_cols, info_.col_count);
                    }

                    template <typename V>
                        Data &operator<<(const V &v)
                        {
                            oss_ << v << ' ';
                            ++info_.col_count;
                            return *this;
                        }
                private:
                    Data(const Data &);
                    Data &operator=(const Data &);

                    std::ostringstream oss_;
                    Info &info_;
            };
            Data data(int ix = 0) { return Data(data_[ix]); }
            Stream &name(int ix, const std::string &n) { data_[ix].name = n; return *this; }

            void stream(std::ostream &os) const
            {
                for (const auto &p: data_)
                {
                    const auto ix = p.first;
                    const auto &lines = p.second.lines;
                    stream_data_name_(os, ix);
                    os << " << EOD" << std::endl;
                    for (const auto &line: lines)
                        os << line << std::endl;
                    os << "EOD" << std::endl;
                }

                OnlyOnce add_plot;
                for (const auto &p: data_)
                {
                    const auto ix = p.first;
                    const auto &info = p.second;
                    const auto nr_cols = info.max_nr_cols;
                    const auto &name = info.name;

                    os << (add_plot() ? "plot " : ", ");
                    stream_data_name_(os, ix);
                    os << " using ";
                    if (nr_cols == 1)
                    {
                        os << 0 << ':' << 1 << " with lines";
                        if (!name.empty())
                            os << " t \"" << name << '\"';
                    }
                    else if (nr_cols == 2)
                    {
                        os << 1 << ':' << 2 << " with lines";
                        if (!name.empty())
                            os << " t \"" << name << '\"';
                    }
                }
                os << std::endl;

                os << "pause mouse" << std::endl;
            }
            std::string str() const
            {
                std::ostringstream oss;
                stream(oss);
                return oss.str();
            }
            void save(const std::string &fn) const
            {
                std::ofstream of(fn);
                stream(of);
            }

        private:
            static void stream_data_name_(std::ostream &os, int ix) { os << "$data_" << ix; }
            std::map<int, Info> data_;
    };

} } 

#endif
