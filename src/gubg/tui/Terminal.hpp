#ifndef HEADER_gubg_tui_Terminal_hpp_ALREAD_INCLUDED
#define HEADER_gubg_tui_Terminal_hpp_ALREAD_INCLUDED

#include <memory>
#include <ostream>

namespace gubg { namespace tui {

    class Terminal
    {
    public:
        Terminal();
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
        bool read(char &);

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> pimpl_;
    };

    std::ostream &operator<<(std::ostream &, const Terminal::CharSize &);

}} // namespace gubg::tui

#endif
