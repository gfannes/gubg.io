#include "gubg/platform.h"

#if GUBG_PLATFORM_API_POSIX
#include "gubg/serial/Endpoint_posix.hxx"
#else
#include "gubg/serial/Endpoint_dummy.hxx"
#endif

