#ifndef HEADER_gubg_serial_Endpoint_hpp_ALREADY_INCLUDED
#define HEADER_gubg_serial_Endpoint_hpp_ALREADY_INCLUDED

#include "gubg/std/cstddef.hpp"
#include <cassert>

namespace gubg { namespace serial { 

    enum class Parity {Odd, Even, None};

    struct Settings
    {
        unsigned int baud_rate = 0;
        unsigned int char_size = 8;
        unsigned int stop_bits = 1;
        Parity parity = Parity::None;
    };

    class Endpoint
    {
    public:
        Endpoint();
        Endpoint(const char *device);
        Endpoint(const char *device, const Settings &settings);
        ~Endpoint();

        bool valid() const { return fd_ >= 0; }

        bool open(const char *device);
        bool open(const char *device, const Settings &settings);

        void close();

        bool get(Settings &settings) const;
        bool set(const Settings &settings);

        template <typename Byte>
        bool send(size_t &offset, const Byte *buffer, size_t size)
        {
            static_assert(sizeof(Byte) == sizeof(std::byte), "Can only send byte-like elements");
            return send_(offset, (const std::byte *)buffer, size);
        }
        template <typename Byte>
        bool receive(size_t &offset, Byte *buffer, size_t size)
        {
            static_assert(sizeof(Byte) == sizeof(std::byte), "Can only receive byte-like elements");
            return receive_(offset, (std::byte *)buffer, size);
        }

    private:
        //No copying allowed
        Endpoint(const Endpoint &);
        Endpoint &operator=(const Endpoint &);

        bool send_(size_t &offset, const std::byte *buffer, size_t size);
        bool receive_(size_t &offset, std::byte *buffer, size_t size);

        int fd_ = -1;
    };

} } 

#endif
