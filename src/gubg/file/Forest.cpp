#include "gubg/file/Forest.hpp"
#include "gubg/file/Filesystem.hpp"
#include <set>
#include <algorithm>
#include <cassert>
using namespace gubg::file;
using namespace gubg::mss;
using namespace std;

//Tree
namespace 
{
    const char * const logns = "file::Forest";
    struct FileAdder
    {
        Tree &tree;

        FileAdder(Tree &t):
            tree(t){}

        ReturnCode recursor_discoveredFile(const File &file)
        {
            MSS_BEGIN(ReturnCode);
            L(C(file));
            tree.add(file);
            MSS_END();
        }
    };
}
Tree::Ptr Tree::create(const File &root, const vector<string> &extensions)
{
    Ptr tree(new Tree(root, extensions));
    FileAdder fileAdder(*tree);
    recurse(fileAdder, root);
    return tree;
}
Tree::Tree(const File &root, const Extensions &exts):
    root_(root),
    extensions_(exts)
{
}

ReturnCode Tree::add(const File &file)
{
    MSS_BEGIN(ReturnCode);
    const auto ext = file.extension();
    MSS_Q(on_fail(find(extensions_.begin(), extensions_.end(), ext) != extensions_.end(), ReturnCode::WrongExtension));
    files_.push_back(file);
    MSS_END();
}

//Forest
ReturnCode Forest::add(const File &root, const Tree::Extensions &extensions)
{
    MSS_BEGIN(ReturnCode);
    L(C(root));
    auto tree = Tree::create(root, extensions);
    MSS(!!tree);
    MSS(on_fail(count(trees_.begin(), trees_.end(), tree) == 0, ReturnCode::TreeAlreadyAdded));
    trees_.push_back(tree);
    MSS_END();
}

Forest::Files Forest::allFiles() const
{
    assert(invariants_());

    Files files;
    for (auto tree: trees_)
    {
        for (const auto &f: tree->files())
            files.push_back(f);
    }
    return files;
}
Forest::Files Forest::allFiles(const Tree::Extensions &exts) const
{
    assert(invariants_());

    //Setup a lookup set
    set<string> extensions;
    for (auto e: exts)
        extensions.insert(e);

    Files files;
    for (auto tree: trees_)
    {
        for (const auto &f: tree->files())
        {
            if (extensions.count(f.extension()))
                files.push_back(f);
        }
    }
    return files;
}
Forest::Files Forest::allRoots() const
{
    assert(invariants_());

    Files files;
    for (auto tree: trees_)
    {
        files.push_back(tree->root());
    }
    return files;
}
ReturnCode Forest::resolve(File &resolved, const File &partial, const size_t overlap) const
{
    File root;
    return resolve(resolved, root, partial, overlap);
}
ReturnCode Forest::resolve(File &resolved, File &root, const File &partial, const size_t overlap) const
{
    assert(invariants_());

	S(logns);L(STREAM(partial, overlap));
    for (auto tree: trees_)
    {
		S(logns);L(STREAM(tree->root()));
        File wanted = tree->root();
        size_t ol = overlap;
        while (ol > 0)
        {
            --ol;
            string bn;
            wanted.popBasename(bn);
        }
		L(STREAM(wanted));
        wanted << partial.name();
		L(STREAM(wanted));
        for (const auto &f: tree->files())
        {
            if (wanted.name() == f.name())
            {
                resolved = f;
                root = tree->root();
                return ReturnCode::OK;
            }
        }
    }
    return ReturnCode::CouldNotResolve;
}
bool Forest::contains(const File &wf) const
{
    for (const auto &f: allFiles())
        if (wf.name() == f.name())
            return true;
    return false;
}

bool Forest::invariants_() const
{
    for (auto tree: trees_)
        if (!tree)
            return false;
    return true;
}
