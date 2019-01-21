#if !GUBG_PLATFORM_API_POSIX
#error This file uses the POSIX API
#endif

#include "gubg/serial/Endpoint.hpp"
#include "gubg/mss.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <array>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

namespace  { 
    unsigned int from_speed(speed_t speed)
    {
        switch (speed)
        {
#define l_case(v) case B ## v: return v
            l_case(0);
            l_case(50);
            l_case(75);
            l_case(110);
            l_case(134);
            l_case(150);
            l_case(200);
            l_case(300);
            l_case(600);
            l_case(1200);
            l_case(1800);
            l_case(2400);
            l_case(4800);
            l_case(9600);
            l_case(19200);
            l_case(38400);
            l_case(57600);
            l_case(115200);
            l_case(230400 );
#undef l_case
        }
        return 0;
    }
    bool to_speed(speed_t &speed, unsigned int baud)
    {
        MSS_BEGIN(bool);
        switch (baud)
        {
#define l_case(v) case v: speed = B ## v; break
            l_case(0);
            l_case(50);
            l_case(75);
            l_case(110);
            l_case(134);
            l_case(150);
            l_case(200);
            l_case(300);
            l_case(600);
            l_case(1200);
            l_case(1800);
            l_case(2400);
            l_case(4800);
            l_case(9600);
            l_case(19200);
            l_case(38400);
            l_case(57600);
            l_case(115200);
            l_case(230400 );
#undef l_case
            default: MSS(false, std::cout << "Error: unsupported baud rate " << baud << std::endl); break;
        }
        MSS_END();
    }
} 

namespace gubg { namespace serial { 

    Endpoint::Endpoint()
    {
    }
    Endpoint::Endpoint(const char *device)
    {
        open(device);
    }
    Endpoint::Endpoint(const char *device, const Settings &settings)
    {
        open(device, settings);
    }
    Endpoint::~Endpoint()
    {
        close();
    }

    bool Endpoint::open(const char *device)
    {
        MSS_BEGIN(bool);
        MSS(!!device);
        close();
        const auto fd = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        /* const auto fd = ::open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC); */
        MSS(fd >= 0, std::cout << "Error: could not open serial port " << device << std::endl);

        struct termios options;
        ::tcgetattr(fd_, &options);
        ::cfmakeraw(&options);

        MSS(::tcsetattr(fd, TCSANOW, &options) == 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        fd_ = fd;
        MSS_END();
    }
    bool Endpoint::open(const char *device, const Settings &settings)
    {
        MSS_BEGIN(bool);
        MSS(open(device));
        MSS(set(settings));
        MSS_END();
    }
    void Endpoint::close()
    {
        if (!valid())
            return;
        ::close(fd_);
        fd_ = -1;
    }

    bool Endpoint::get(Settings &settings) const
    {
        MSS_BEGIN(bool);

        MSS(valid());

        struct termios options;
        ::tcgetattr(fd_, &options);

        {
            const auto input_speed = ::cfgetispeed(&options);
            const auto output_speed = ::cfgetospeed(&options);
            MSS(input_speed == output_speed);
            settings.baud_rate = from_speed(input_speed);
        }

        {
            const auto char_size = options.c_cflag & CSIZE;
            switch (char_size)
            {
                case CS5: settings.char_size = 5; break;
                case CS6: settings.char_size = 6; break;
                case CS7: settings.char_size = 7; break;
                case CS8: settings.char_size = 8; break;
                default: MSS(false, std::cout << "Error: unknown char size " << char_size << std::endl); break;
            }
        }

        {
            settings.stop_bits = (options.c_cflag & CSTOPB ? 2 : 1);
        }

        {
            settings.parity = (options.c_cflag & PARENB ? (options.c_cflag & PARODD ? Parity::Odd : Parity::Even) : Parity::None);
        }

        MSS_END();
    }
    bool Endpoint::set(const Settings &settings)
    {
        MSS_BEGIN(bool);

        MSS(valid());

        struct termios options;
        ::tcgetattr(fd_, &options);

        {
            speed_t speed;
            MSS(to_speed(speed, settings.baud_rate));
            ::cfsetispeed(&options, speed);
            ::cfsetospeed(&options, speed);
        }

        {
            options.c_cflag &= ~CSIZE;
            switch (settings.char_size)
            {
                case 5: options.c_cflag |= CS5; break;
                case 6: options.c_cflag |= CS6; break;
                case 7: options.c_cflag |= CS7; break;
                case 8: options.c_cflag |= CS8; break;
                default: MSS(false, std::cout << "Error: unsupported char size " << settings.char_size << std::endl); break;
            }
        }

        {
            switch (settings.stop_bits)
            {
                case 1: options.c_cflag &= ~CSTOPB; break;
                case 2: options.c_cflag |=  CSTOPB; break;
                default: MSS(false, std::cout << "Error: unsupported stop bits " << settings.stop_bits << std::endl); break;
            }
        }

        {
            switch (settings.parity)
            {
                case Parity::Odd:
                    options.c_cflag |= PARENB;
                    options.c_cflag |= PARODD;
                    break;
                case Parity::Even:
                    options.c_cflag |= PARENB;
                    options.c_cflag &= ~PARODD;
                    break;
                case Parity::None:
                    options.c_cflag &= ~PARENB;
                    break;
                default: MSS(false, std::cout << "Error: unsupported parity " << (int)settings.parity << std::endl); break;
            }
        }

        MSS(::tcsetattr(fd_, TCSANOW, &options) == 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        MSS_END();
    }

    bool Endpoint::flush_receive()
    {
        MSS_BEGIN(bool);
        std::array<std::byte, 1024> buffer;
        while (true)
        {
            size_t offset = 0;
            MSS(receive(offset, buffer.data(), buffer.size()));
            if (offset == 0)
                MSS_RETURN_OK();
        }
        MSS_END();
    }

    //Privates
    bool Endpoint::send_(size_t &offset, const std::byte *buffer, size_t size)
    {
        MSS_BEGIN(bool);
        MSS(!!buffer);
        MSS(valid());
        const auto nr_to_send = size-offset;
        const auto nr_sent = ::write(fd_, buffer+offset, nr_to_send);
        MSS(nr_sent >= 0);
        offset += nr_sent;
        MSS_END();
    }
    bool Endpoint::receive_(size_t &offset, std::byte *buffer, size_t size)
    {
        MSS_BEGIN(bool);
        MSS(!!buffer);
        MSS(valid());
        const auto nr_to_receive = size-offset;
        const auto nr_received = ::read(fd_, buffer+offset, nr_to_receive);
        if (nr_received == -1)
            MSS((errno == EAGAIN || errno == EWOULDBLOCK), std::cout << "Error: failed to read: " << C(errno) << std::endl);
        else
            offset += nr_received;
        MSS_END();
    }

} } 
