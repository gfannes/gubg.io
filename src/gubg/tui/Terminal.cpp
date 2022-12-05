#include <gubg/tui/Terminal.hpp>

#include <gubg/mss.hpp>
#include <gubg/naft/Document.hpp>

#include <oof.h>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <optional>

namespace gubg { namespace tui {

    struct Terminal::Pimpl
    {
        std::optional<struct termios> old;

        Pimpl(const Config &config)
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

                if (config.timeout)
                {
                    my.c_cc[VMIN] = 1;
                    my.c_cc[VTIME] = 0;
                }
                else
                {
                    my.c_cc[VMIN] = 0;
                    my.c_cc[VTIME] = 1;
                }

                ::tcsetattr(0, TCSANOW, &my);
            }

            std::cout << oof::cursor_visibility(false) << std::flush;
        }
        ~Pimpl()
        {
            if (old)
                ::tcsetattr(0, TCSADRAIN, &old.value());

            std::cout << oof::cursor_visibility(true) << oof::reset_formatting() << std::flush;
        }
    };

    Terminal::Terminal(const Config &config)
        : pimpl_(new Pimpl{config})
    {
    }

    Terminal::Terminal()
        : Terminal(Config{})
    {
    }

    Terminal::Terminal(Terminal &&dying)
    {
        pimpl_.swap(dying.pimpl_);
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

    bool Terminal::read(std::optional<char> &ch)
    {
        MSS_BEGIN(bool);

        const auto rc = ::read(0, &ch.emplace(), 1);
        MSS(rc >= 0);
        if (rc == 0)
            ch.reset();

        MSS_END();
    }

    void Terminal::clear()
    {
        std::cout << oof::clear_screen() << std::flush;
    }

    void Terminal::print(std::string_view str, const Position &pos, const Style &style, int max_size)
    {
        std::cout << oof::position(pos.row, pos.col) << style << str.substr(0, max_size >= 0 ? max_size : std::string::npos) << std::flush;
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
