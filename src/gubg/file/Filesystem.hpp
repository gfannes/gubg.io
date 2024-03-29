#ifndef HEADER_gubg_file_Filesystem_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Filesystem_hpp_ALREADY_INCLUDED

#include <gubg/file/File.hpp>
#include <gubg/file/Codes.hpp>
#include <gubg/mss.hpp>
#include <string>
#include <filesystem>
#include <vector>

namespace gubg { namespace file {

    //Returns the size of File
    ReturnCode size(size_t &, const File &);

    //Reads the content of a File
    ReturnCode read(std::string &content, const File &);
    ReturnCode read(std::string &content, const std::filesystem::path &);
    ReturnCode read(std::string &content, const std::string &);
    ReturnCode read(std::vector<File> &, const File &);

    //Write a file
    ReturnCode write(const std::string &content, const File &);
    ReturnCode write(const std::string &content, const std::filesystem::path &);
    ReturnCode write(const std::string &content, const std::string &);

    //Remove a file
    ReturnCode remove(const File &);

    //Copy a file
    ReturnCode copy(const File &from, const File &to);

    ReturnCode determineType(File &file);

    ReturnCode resolve(File &file);

    bool exists(const File &);
    bool isRegular(const File &);
    bool isDirectory(const File &);

    ReturnCode mkdir(const File &);

    ReturnCode getcwd(File &);
    File getcwd();

    enum Flags: unsigned char {Read = 0x01, Write = 0x02, Execute = 0x4, ReadWrite = (Read|Write), All = (Read|Write|Execute)};
    struct Mode
    {
        Flags user;
        Flags group;
        Flags other;
    };
    ReturnCode chmod(const File &, const Mode &);

    //Calls recursor_discoveredFile(file) to indicate a new file was discovered
    //* OK: continue
    //* Skip: skip this file
    //* Stop: stop recursing
    namespace priv
    {
        template <typename Functor>
            ReturnCode recurse(Functor &ftor, const File &file)
            {
                MSS_BEGIN(ReturnCode);
                typedef decltype(ftor.recursor_discoveredFile(file)) RC;
                switch (auto rc = ftor.recursor_discoveredFile(file))
                {
                    case RC::OK:
                        {
                            std::vector<File> files;
                            if (isDirectory(file))
                            {
                                MSS(read(files, file));
                                for (auto ff: files)
                                {
                                    switch (auto rc2 = priv::recurse(ftor, ff))
                                    {
                                        case ReturnCode::Stop: MSS_Q(rc2); break;
                                        default:               MSS(rc2); break;
                                    }
                                }
                            }
                        }
                        break;
                    case RC::Skip:          break;
                    case RC::Stop: MSS_Q(ReturnCode::Stop); break;
                    default:       MSS(rc); break;
                }
                MSS_END();
            }
    }
    template <typename Functor>
        ReturnCode recurse(Functor &ftor, const File &file)
        {
            MSS_BEGIN(ReturnCode);
            File ff(file);
            MSS(determineType(ff));
            MSS(priv::recurse(ftor, ff));
            MSS_END();
        }
    template <typename Functor>
        ReturnCode recurse(Functor &ftor)
        {
            return recurse(ftor, getcwd());
        }

} }

#endif
