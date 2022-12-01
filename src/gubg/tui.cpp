#include <gubg/mss.hpp>
#include <gubg/tui.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace gubg {
namespace tui {

bool get_size(unsigned int &width, unsigned int &height)
{
  MSS_BEGIN(bool);

  struct winsize sz{};
  MSS(::ioctl(0, TIOCGWINSZ, &sz) == 0);

  width = sz.ws_col;
  height = sz.ws_row;

  MSS_END();
}

bool get_char(char &ch) {
  MSS_BEGIN(bool);

  struct termios old = {0};
  MSS(::tcgetattr(0, &old) == 0);

  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  MSS(::tcsetattr(0, TCSANOW, &old) == 0);

  MSS(read(0, &ch, 1) > 0);

  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  MSS(::tcsetattr(0, TCSADRAIN, &old) == 0);

  MSS_END();
}

} // namespace tui
} // namespace gubg