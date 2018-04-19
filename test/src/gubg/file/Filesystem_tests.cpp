#include "catch.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg::file;
using namespace gubg::mss;
using namespace std;

namespace 
{
    auto logns = "test";
    bool countNrFiles(size_t &count, const File &f)
    {
        MSS_BEGIN(bool);
        //L("(" << f.type() << ") " << f.name());
        ++count;
        vector<File> files;
        MSS(ReturnCode::OK == read(files, f));
        for (auto ff: files)
            if (isDirectory(ff))
                countNrFiles(count, ff);
        MSS_END();
    }


}

TEST_CASE("FilesystemTests", "[ut]")
{
    S(logns);
    SECTION("resolve")
    {
        File file(".");
        REQUIRE(is_ok(resolve(file)));
        L(file);
    }
    SECTION("getcwd")
    {
        L(gubg::file::getcwd().name());
    }
    SECTION("read")
    {
        SECTION("regular")
        {
            const File f(__FILE__);
            size_t s;
            REQUIRE(is_ok(size(s, f)));
            L("File size for " << f.name() << ": " << s << " bytes");
            string content;
            REQUIRE(is_ok(read(content, f)));
            L(content);
        }
#if 0
        SECTION("directory")
        {
            const File root("/");
            const File home("/home/gfannes");
            const File gubg("/home/gfannes/gubg");
            vector<File> files;
            REQUIRE(is_ok(read(files, home)));
            L("I found " << files.size() << " files in " << home.name());
            for (auto f: files)
                cout << f.name() << "(" << f.type() << "), ";
            cout << endl;

            SECTION("countNrFiles")
            {
                size_t nrFiles = 0;
                countNrFiles(nrFiles, gubg);
                L("I found " << nrFiles << " files and dirs in " << gubg.name());
            }
            SECTION("recurse")
            {
                struct Counter
                {
                    size_t count;
                    Counter():count(0){}
                    ReturnCode recursor_discoveredFile(const File &file)
                    {
                        MSS_BEGIN(ReturnCode, file);
                        ++count;
                        //MSS(count < 10, Stop);
                        //MSS(count < 10, Skip);
                        MSS_END();
                    }
                };
                Counter counter;
                recurse(counter, gubg);
                L("Filecount: " << counter.count);
            }
        }
#endif
    }
    SECTION("remove")
    {
        File file("remove_me.txt");
        REQUIRE(is_ok(write("abc", file)));
        REQUIRE(is_ok(remove(file)));
        REQUIRE(!is_ok(remove(file)));
    }
    SECTION("copy")
    {
        File file("remove_me.txt");
        REQUIRE(is_ok(write("abc", file)));
        File file2("remove_me_2.txt");
        REQUIRE(is_ok(copy(file, file2)));
        REQUIRE(is_ok(remove(file)));
        REQUIRE(is_ok(remove(file2)));
    }
    SECTION("mkdir")
    {
        File dir("test_dir/aaa");
        if (isDirectory(dir))
        {
            REQUIRE(is_ok(remove(dir)));
        }
        REQUIRE(!isDirectory(dir));
        REQUIRE(is_ok(mkdir(dir)));
        REQUIRE(isDirectory(dir));
        //remove(dir);
    }
}


