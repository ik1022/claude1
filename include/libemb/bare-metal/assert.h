#pragma once

// Bare-metal assert for embedded systems
#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) ((void)0)  // Can be customized with halt() for debugging
#endif
