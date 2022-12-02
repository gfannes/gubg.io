#include <gubg/tui/Terminal.hpp>

#include <gubg/mss.hpp>
#include <gubg/naft/Document.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <optional>

namespace gubg { namespace tui {

    struct Terminal::Pimpl
    {
        std::optional<struct termios> old;

        Pimpl()
        {
            if (::tcgetattr(0, &old.emplace()) != 0)
                old.reset();
            else
            {
                auto my = *old;

                // Canonical mode: do not wait for <enter>
                my.c_lflag &= ~ICANON;
                // Do not echo each typed character
                my.c_lflag &= ~ECHO;

                my.c_cc[VMIN] = 1;
                my.c_cc[VTIME] = 0;

                ::tcsetattr(0, TCSANOW, &my);
            }
        }
        ~Pimpl()
        {
            if (old)
                ::tcsetattr(0, TCSADRAIN, &old.value());
        }
    };

    Terminal::Terminal(): pimpl_(new Pimpl{})
    {
    }

    Terminal::~Terminal()
    {
    }

    bool Terminal::get(CharSize &size)
    {
        MSS_BEGIN(bool);

        struct winsize sz;
        MSS(::ioctl(0, TIOCGWINSZ, &sz) == 0);

        size.width = sz.ws_col;
        size.height = sz.ws_row;

        MSS_END();
    }

    bool Terminal::read(char &ch)
    {
        MSS_BEGIN(bool);

        MSS(::read(0, &ch, 1) > 0);

        MSS_END();
    }

    std::ostream &operator<<(std::ostream &os, const Terminal::CharSize &size)
    {
        naft::Document doc{os};
        auto n = doc.node("Terminal::CharSize");
        n.attr("width_char", size.width);
        n.attr("height_char", size.height);
        return os;
    }

}} // namespace gubg::tui
