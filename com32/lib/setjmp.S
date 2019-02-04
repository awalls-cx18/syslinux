#ifdef __i386__
#include <i386/setjmp.S>
#elif defined(__x86_64__)
#include <x86_64/setjmp.S>
#else
#error "Unable to build for to-be-defined architecture type"
#endif

