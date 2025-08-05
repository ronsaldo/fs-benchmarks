#include "Common.h"

#ifdef _WIN32
#error TODO: Windows
#else
#include <time.h>

int64_t getCurrentMicroseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec*1000000 + ts.tv_nsec / 1000;
}
#endif