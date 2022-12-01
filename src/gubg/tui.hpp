#ifndef HEADER_gubg_tui_hpp_ALREAD_INCLUDED
#define HEADER_gubg_tui_hpp_ALREAD_INCLUDED

namespace gubg {
namespace tui {

// Read a character without waiting for <enter> nor echoing the character to the terminal
bool get_char(char &);

// Retrieve the terminal size
bool get_size(unsigned int &width, unsigned int &height);

}
} // namespace gubg

#endif
