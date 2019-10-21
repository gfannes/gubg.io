#include <app/output.hpp>
#include <app/App.hpp>

int main(int argc, const char **argv)
{
    MSS_BEGIN(int);

    app::App app;

    MSS(app.process(argc, argv), app::error() << "Could not process the CLI arguments\n");
    MSS(app.run(), app::error() << "Could not run\n");

    MSS_END();
}
