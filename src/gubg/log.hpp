#ifdef TAG
#undef TAG_1
#undef TAG_2
#undef TAG
#endif

#ifdef ATTR
#undef ATTR
#endif

#if gubg_no_log
#include "gubg/log/macros_noop.hpp"
#else
#include "gubg/log/macros.hpp"
#endif
