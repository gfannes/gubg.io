#ifndef HEADER_gubg_tui_Terminal_hpp_ALREAD_INCLUDED
#define HEADER_gubg_tui_Terminal_hpp_ALREAD_INCLUDED

#include <gubg/tui/Style.hpp>

#include <memory>
#include <optional>
#include <ostream>
#include <string>

namespace gubg { namespace tui {

    struct Position
    {
        unsigned int row{};
        unsigned int col{};
    };

    class Terminal
    {
    public:
        struct Config
        {
            bool timeout = false;
        };

        Terminal(const Config &);
        Terminal();
        Terminal(Terminal &&);
        ~Terminal();

        // Retrieve the terminal size in chars
        struct CharSize
        {
            unsigned int width{};
            unsigned int height{};
        };
        bool get(CharSize &);

        // Read a character without waiting for <enter> nor echoing the character to
        // the terminal
        bool read(std::optional<char> &);

        void clear();

        void print(std::string_view, const Position &, const Style & = {}, int max_size = -1);

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> pimpl_;
    };

    std::ostream &operator<<(std::ostream &, const Terminal::CharSize &);

}} // namespace gubg::tui

#endif
