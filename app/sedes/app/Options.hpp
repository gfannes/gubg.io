#ifndef HEADER_app_Options_hpp_ALREADY_INCLUDED
#define HEADER_app_Options_hpp_ALREADY_INCLUDED

#include <app/output.hpp>
#include <gubg/file/Name.hpp>
#include <gubg/mss.hpp>
#include <sstream>

namespace app { 

    class Options
    {
    public:
        std::string application_name;
        bool print_help = false;
        gubg::file::Name input_fn;
        gubg::file::Name output_fn;

        bool parse(int argc, const char **argv)
        {
            MSS_BEGIN(bool);
            unsigned int ix = 0;
            auto pop = [&]() {
                if (ix >= argc)
                    return std::string();
                return std::string(argv[ix++]);
            };

            application_name = pop();

            for (; ix < argc; )
            {
                const auto arg = pop();
                if (false) {}
                else if (arg == "-h" || arg == "--help") {print_help = true;}
                else if (arg == "-i" || arg == "--input") {input_fn = pop();}
                else if (arg == "-o" || arg == "--output") {output_fn = pop();}
                else MSS(false, error() << "Unknown argument \"" << arg << "\"\n");
            }
            MSS_END();
        }
    protected:
        std::string help() const
        {
            std::ostringstream oss;
            oss << application_name << " [options]" << std::endl;
            oss << "    -h/--help   Print this help" << std::endl;
            oss << "    -i/--input  Input filename" << std::endl;
            oss << "    -o/--output Output filename" << std::endl;
            /* oss << "" << std::endl; */
            return oss.str();
        }
    private:
    };

} 

#endif
