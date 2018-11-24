#include "gubg/serial/Endpoint.hpp"

namespace gubg { namespace serial { 

    Endpoint::Endpoint()
    {
    }
    Endpoint::Endpoint(const char *device)
    {
    }
    Endpoint::Endpoint(const char *device, const Settings &settings)
    {
    }
    Endpoint::~Endpoint()
    {
    }

    bool Endpoint::open(const char *device)
    {
        return false;
    }
    bool Endpoint::open(const char *device, const Settings &settings)
    {
        return false;
    }

    void Endpoint::close()
    {
    }

    bool Endpoint::get(Settings &settings) const
    {
        return false;
    }
    bool Endpoint::set(const Settings &settings)
    {
        return false;
    }

    bool Endpoint::send_(size_t &offset, const std::byte *buffer, size_t size)
    {
        return false;
    }
    bool Endpoint::receive_(size_t &offset, std::byte *buffer, size_t size)
    {
        return false;
    }
} } 
