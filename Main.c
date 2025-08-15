#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t randomSeed;
static uint32_t *randomData;
static uint32_t *readDataBuffer;
static size_t randomDataSize = 1000000;

void setRandomNumberSeed(uint32_t seed)
{
    randomSeed = seed;
}

uint32_t nextRandomNumber()
{
    return randomSeed = (randomSeed*1103515245 + 12345) &0x7fffffff;
}

void benchmarkFileApi(file_api_t *api)
{
    int writeBufferCount = 200;
    int readBufferCount = writeBufferCount;

    //file_api_file_t *file = api->open("benchmark-file", true);
    file_api_file_t *file = api->openWriteWithSize("benchmark-file", writeBufferCount*randomDataSize*4);

    // Write data tests
    {
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < writeBufferCount; ++i)
            api->write(file, randomDataSize*4, randomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        double writeTime = (double)(writeEndingTime - writeStartingTime) / writeBufferCount * 0.001;
        printf("%s. First average write time(ms): %f\n", api->name, writeTime);
    }

    // Write data tests
    {
        api->seek(file, 0);
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < writeBufferCount; ++i)
            api->write(file, randomDataSize*4, randomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        double writeTime = (double)(writeEndingTime - writeStartingTime) / writeBufferCount * 0.001;
        printf("%s. Second average write time(ms): %f\n", api->name, writeTime);
    }

    // Read data test
    {
        api->seek(file, 0);
        int64_t readStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < readBufferCount; ++i)
            api->read(file, randomDataSize*4, readDataBuffer);
        int64_t readEndingTime = getCurrentMicroseconds();
        double readTime = (double)(readEndingTime - readStartingTime) / readBufferCount * 0.001;
        printf("%s. Average read time(ms): %f\n", api->name, readTime);
    }

    api->close(file);
}

int main(int argc, const char *argv[])
{
    initializeClock();
    setRandomNumberSeed(42);

    // Generate random data buffer.
    randomData = malloc(sizeof(uint32_t)*randomDataSize);
    readDataBuffer = malloc(sizeof(uint32_t)*randomDataSize);
    for(int i = 0; i < randomDataSize; ++i)
        randomData[i] = nextRandomNumber();

    benchmarkFileApi(&stdio_file_api);
#ifdef _WIN32
    benchmarkFileApi(&win32_file_api);
    benchmarkFileApi(&win32_overlapped_file_api);
#else
    benchmarkFileApi(&posix_file_api);
    benchmarkFileApi(&posix_explicit_offset_file_api);
    benchmarkFileApi(&posix_mmap_file_api);
#endif
    
    free(randomData);
    free(readDataBuffer);
    return 0;
}
