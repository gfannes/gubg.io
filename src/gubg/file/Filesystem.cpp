#include "gubg/file/Filesystem.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "gubg/platform.h"
#if GUBG_PLATFORM_API_POSIX
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
    #include <stdlib.h>
#endif
#if GUBG_PLATFORM_API_WIN32
    #include <direct.h>
    #include <errno.h>
    #include <stdio.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <time.h>
    #include "Windows.h"
#endif

namespace gubg { namespace file {

        ReturnCode size(size_t &fileSize, const File &file)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mss::on_fail(File::Unknown == file.type() || File::Regular == file.type(), ReturnCode::ExpectedRegular));
            std::ifstream fi(file.name(), std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
            MSS(mss::on_fail(bool(fi), ReturnCode::CouldNotOpenFile));
            fileSize = fi.tellg();
            MSS_END();
        }

        ReturnCode read(std::string &content, const File &file)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mss::on_fail(File::Unknown == file.type() || File::Regular == file.type(), ReturnCode::ExpectedRegular));
            MSS(read(content, file.name()));
            MSS_END();
        }

        ReturnCode read(std::string &content, const std::filesystem::path &fp)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mss::on_fail(std::filesystem::is_regular_file(fp), ReturnCode::ExpectedRegular));
            MSS(read(content, fp.string()));
            MSS_END();
        }

        ReturnCode read(std::string &content, const std::string &filename)
        {
            MSS_BEGIN(ReturnCode);
            std::ifstream fi(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
            MSS(mss::on_fail(bool(fi), ReturnCode::CouldNotOpenFile));
            const auto byte_size = fi.tellg();
            content.resize(byte_size);
            fi.seekg(0);
            fi.read(&content[0], byte_size);
            MSS_END();
        }

#if GUBG_PLATFORM_API_POSIX
        namespace {
            struct Dir
            {
                DIR *h;
                Dir(DIR *d)
                    : h(d) {}
                ~Dir()
                {
                    if (h)
                        ::closedir(h);
                }
            };
        } // namespace
#endif
#if GUBG_PLATFORM_API_MINGW
          // Some things are missing for MinGW
    #define NAME_MAX FILENAME_MAX
        int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
        {
            std::mutex m;
            std::lock_guard<std::mutex> lock(m);
            struct dirent *p;
            if (!(p = ::readdir(dirp)))
            {
                *result = 0;
                return 0;
            }
            *entry = *p;
            *result = entry;
            return 0;
        }
#endif
        ReturnCode read(std::vector<File> &files, const File &file)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mss::on_fail(File::Unknown == file.type() || File::Directory == file.type(), ReturnCode::ExpectedDirectory));
#if GUBG_PLATFORM_API_POSIX
            // Open the directory in a RAII
            Dir dir(::opendir(file.name().c_str()));
            MSS(mss::on_fail(!!dir.h, ReturnCode::CouldNotOpenDir));
            // Create enough space for the dirent struct
            std::string entrypBuffer(offsetof(struct dirent, d_name) + NAME_MAX + 1, '0');
            struct dirent *entryp = (struct dirent *)entrypBuffer.data();
            // Iterate over the directory entries
            struct dirent *tmp;
            for (;;)
            {
                MSS(mss::on_fail(0 == ::readdir_r(dir.h, entryp, &tmp), ReturnCode::CouldNotReadEntry));
                if (!tmp)
                    // If this is set to 0, we are done with iterating
                    break;
                if (::strcmp(entryp->d_name, ".") == 0 || ::strcmp(entryp->d_name, "..") == 0)
                    // We skip "." and ".."
                    continue;

                {
                    File::Type type = File::Unknown;
    #if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
                    switch (entryp->d_type)
                    {
                        case DT_DIR: type = File::Directory; break;
                        case DT_REG: type = File::Regular; break;
                        case DT_LNK: type = File::Symbolic; break;
                        case DT_FIFO: type = File::FIFO; break;
                    }
                    if (File::Unknown != type)
                        files.push_back(File(file).setType(type) << entryp->d_name);
    #else
                    // MinGW does not have the dirent.d_type data member
                    {
                        File f(file);
                        f << entryp->d_name;
                        if (mss::is_ok(determineType(f)))
                            files.push_back(f);
                    }
    #endif
                }
            }
#else
            HANDLE hFind;
            WIN32_FIND_DATA FindFileData;

            File pattern(file);
            pattern << "*";
            if ((hFind = FindFirstFile(pattern.name().c_str(), &FindFileData)) != INVALID_HANDLE_VALUE)
            {
                do
                {
                    File f(file);
                    f << FindFileData.cFileName;
                    const auto bn = f.basename();
                    if (bn == "." || bn == "..")
                        // We skip "." and ".."
                        continue;
                    if (mss::is_ok(determineType(f)))
                        files.push_back(f);
                } while (FindNextFile(hFind, &FindFileData));
                FindClose(hFind);
            }
#endif
            MSS_END();
        }

        ReturnCode write(const std::string &content, const File &file)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mss::on_fail(File::Unknown == file.type() || File::Regular == file.type(), ReturnCode::ExpectedRegular));
            std::ofstream fo(file.name(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            fo.write(content.data(), content.size());
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            MSS_END();
        }
        ReturnCode write(const std::string &content, const std::filesystem::path &fp)
        {
            MSS_BEGIN(ReturnCode);
            std::ofstream fo(fp, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            fo.write(content.data(), content.size());
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            MSS_END();
        }
        ReturnCode write(const std::string &content, const std::string &fp)
        {
            MSS_BEGIN(ReturnCode);
            std::ofstream fo(fp, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            fo.write(content.data(), content.size());
            MSS(mss::on_fail(bool(fo), ReturnCode::CouldNotWriteFile));
            MSS_END();
        }

        ReturnCode remove(const File &file)
        {
            MSS_BEGIN(ReturnCode);
            MSS_Q(mss::on_fail(exists(file), ReturnCode::FileDoesNotExist));
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            MSS(mss::on_fail(::remove(file.name().c_str()) == 0, ReturnCode::CouldNotRemove));
#else
            auto f = file;
            MSS(determineType(f));
            switch (f.type())
            {
                case File::Regular:
                    MSS(mss::on_fail(::remove(file.name().c_str()) == 0, ReturnCode::CouldNotRemove));
                    break;
                case File::Directory:
                    MSS(mss::on_fail(!!::RemoveDirectory(file.name().c_str()), ReturnCode::CouldNotRemove));
                    break;
                default:
                    MSS(ReturnCode::UnknownFileType);
                    break;
            }
#endif
            MSS_END();
        }

        ReturnCode copy(const File &from, const File &to)
        {
            MSS_BEGIN(ReturnCode);
            std::string content;
            MSS(read(content, from));
            MSS(write(content, to));
            MSS_END();
        }

        ReturnCode determineType(File &file)
        {
            MSS_BEGIN(ReturnCode);
#if GUBG_PLATFORM_COMPILER_MSVC
            struct _stat statbuf;
#else
            struct stat statbuf;
#endif
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            auto res = ::lstat(file.name().c_str(), &statbuf);
#else
    #if GUBG_PLATFORM_COMPILER_GCC
            auto res = ::stat(file.name().c_str(), &statbuf);
    #endif
    #if GUBG_PLATFORM_COMPILER_MSVC
            auto res = _stat(file.name().c_str(), &statbuf);
    #endif
#endif
            if (res != 0)
            {
                L("File \"" << file.name() << "\" does not exist");
                MSS(ReturnCode::FileDoesNotExist);
            }
            switch (statbuf.st_mode & S_IFMT)
            {
                case S_IFREG: file.setType(File::Regular); break;
                case S_IFDIR: file.setType(File::Directory); break;
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
                case S_IFIFO: file.setType(File::FIFO); break;
                case S_IFLNK: file.setType(File::Symbolic); break;
#endif
                default: MSS(ReturnCode::UnknownFileType); break;
            }
            MSS_END();
        }

        ReturnCode resolve(File &file)
        {
            MSS_BEGIN(ReturnCode);
#if GUBG_PLATFORM_COMPILER_MSVC
            const size_t path_max = MAX_PATH;
#else
            const size_t path_max = PATH_MAX;
#endif
            char buffer[path_max];
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            MSS(!!::realpath(file.name().c_str(), buffer));
#endif
#if GUBG_PLATFORM_API_WIN32
            const auto len = ::GetFullPathName(file.name().c_str(), path_max, buffer, 0);
            MSS(len < path_max);
#endif
            file.setName(buffer);
            MSS(determineType(file));
            MSS_END();
        }

        bool exists(const File &file)
        {
            struct stat statbuf;
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            if (0 != ::lstat(file.name().c_str(), &statbuf))
                return false;
#else
            if (0 != ::stat(file.name().c_str(), &statbuf))
                return false;
#endif
            return true;
        }
        bool isRegular(const File &file)
        {
            struct stat statbuf;
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            if (0 != ::lstat(file.name().c_str(), &statbuf))
                return false;
#else
            if (0 != ::stat(file.name().c_str(), &statbuf))
                return false;
#endif
            return (statbuf.st_mode & S_IFMT) == S_IFREG;
        }
        bool isDirectory(const File &file)
        {
            struct stat statbuf;
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            if (0 != ::lstat(file.name().c_str(), &statbuf))
                return false;
#else
            if (0 != ::stat(file.name().c_str(), &statbuf))
                return false;
#endif
            return (statbuf.st_mode & S_IFMT) == S_IFDIR;
        }

        namespace {
            ReturnCode mkdir_recursive_(File dir)
            {
                MSS_BEGIN(ReturnCode);
                if (isDirectory(dir))
                    MSS_RETURN_OK();
                std::string bn;
                MSS(dir.popBasename(bn));
                if (!dir.empty())
                    MSS(mkdir_recursive_(dir));
                dir << bn;
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
                const auto rc = ::mkdir(dir.name().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                MSS(rc == 0);
#else
                MSS(!!::CreateDirectory(dir.name().c_str(), nullptr));
#endif
                MSS_END();
            }
        } // namespace
        ReturnCode mkdir(const File &dir)
        {
            MSS_BEGIN(ReturnCode);
            MSS(mkdir_recursive_(dir));
            MSS_END();
        }

        ReturnCode getcwd(File &file)
        {
            MSS_BEGIN(ReturnCode);
            const size_t size = 4096;
            std::string cwd(size, '\0');
#if GUBG_PLATFORM_COMPILER_MSVC
            MSS(mss::on_fail(0 != _getcwd(&cwd[0], size), ReturnCode::CouldNotGetCWD));
#else
            MSS(mss::on_fail(0 != ::getcwd(&cwd[0], size), ReturnCode::CouldNotGetCWD));
#endif
            cwd.resize(strlen(&cwd[0]));
            file.setName(cwd);
            MSS(determineType(file));
            MSS_END();
        }
        File getcwd()
        {
            File file;
            if (!mss::is_ok(getcwd(file)))
                return File();
            return file;
        }

        ReturnCode chmod(const File &file, const Mode &mode)
        {
            MSS_BEGIN(ReturnCode);
#if GUBG_PLATFORM_OS_LINUX || GUBG_PLATFORM_OS_OSX
            mode_t m = 0;
            if (mode.user & Read) m |= S_IRUSR;
            if (mode.user & Write) m |= S_IWUSR;
            if (mode.user & Execute) m |= S_IXUSR;
            if (mode.group & Read) m |= S_IRGRP;
            if (mode.group & Write) m |= S_IWGRP;
            if (mode.group & Execute) m |= S_IXGRP;
            if (mode.other & Read) m |= S_IROTH;
            if (mode.other & Write) m |= S_IWOTH;
            if (mode.other & Execute) m |= S_IXOTH;
            const auto ret = ::chmod(file.name().c_str(), m);
            MSS(ret == 0);
#endif
            MSS_END();
        }

}} // namespace gubg::file
