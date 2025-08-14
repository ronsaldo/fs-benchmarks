#include "Common.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static LARGE_INTEGER performanceCounterFrequency;

void initializeClock(void)
{
    QueryPerformanceFrequency(&performanceCounterFrequency);
}

int64_t getCurrentMicroseconds()
{
    LARGE_INTEGER performanceCounter;
    if(!QueryPerformanceCounter(&performanceCounter))
        return 0;

    return performanceCounter.QuadPart * 1000000 / performanceCounterFrequency.QuadPart;
}

#else
#include <time.h>

void initializeClock(void)
{
    // Nothing required here
}

int64_t getCurrentMicroseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec*1000000 + ts.tv_nsec / 1000;
}
#endif