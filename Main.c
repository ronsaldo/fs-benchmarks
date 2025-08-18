#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t randomSeed;
static uint32_t *randomData;
static uint32_t *readDataBuffer;
static size_t randomDataSize = 1000000;

static uint32_t *smallRandomData;
static uint32_t *smallReadDataBuffer;
static size_t smallRandomDataSize = 2048;

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
    int smallWriteBufferCount = 10000;
    int smallReadBufferCount = smallWriteBufferCount;

    int writeBufferCount = 200;
    int readBufferCount = writeBufferCount;

    //file_api_file_t *file = api->open("benchmark-file", true);
    file_api_file_t *file = api->openWriteWithSize("benchmark-file", writeBufferCount*randomDataSize*4);

    // Write data tests
    {
        api->seek(file, 0);
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < smallWriteBufferCount; ++i)
            api->write(file, smallRandomDataSize*4, smallRandomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        int64_t deltaTime = writeEndingTime - writeStartingTime;
        double writingSpeed = smallWriteBufferCount * smallRandomDataSize*4 / (double)(deltaTime*1e-6); 
        printf("%s. First small writing speed (MB/s): %f\n", api->name, writingSpeed*0.000001);
    }

    // Write data tests
    {
        api->seek(file, 0);
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < smallWriteBufferCount; ++i)
            api->write(file, smallRandomDataSize*4, smallRandomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        int64_t deltaTime = writeEndingTime - writeStartingTime;
        double writingSpeed = smallWriteBufferCount * smallRandomDataSize*4 / (double)(deltaTime*1e-6); 
        printf("%s. Second small writing speed (MB/s): %f\n", api->name, writingSpeed*0.000001);
    }

    // Write data tests
    {
        api->seek(file, 0);
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < writeBufferCount; ++i)
            api->write(file, randomDataSize*4, randomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        int64_t deltaTime = writeEndingTime - writeStartingTime;
        double writingSpeed = writeBufferCount * randomDataSize*4 / (double)(deltaTime*1e-6); 
        printf("%s. First average writing speed (MB/s): %f\n", api->name, writingSpeed*0.000001);
    }

    // Write data tests
    {
        api->seek(file, 0);
        int64_t writeStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < writeBufferCount; ++i)
            api->write(file, randomDataSize*4, randomData);
        int64_t writeEndingTime = getCurrentMicroseconds();
        int64_t deltaTime = writeEndingTime - writeStartingTime;
        double writingSpeed = writeBufferCount * randomDataSize*4 / (double)(deltaTime*1e-6); 
        printf("%s. Second average writing speed (MB/s): %f\n", api->name, writingSpeed*0.000001);
    }

    // Read data test
    {
        api->seek(file, 0);
        int64_t readStartingTime = getCurrentMicroseconds();
        for(int i = 0; i < readBufferCount; ++i)
            api->read(file, randomDataSize*4, readDataBuffer);
        int64_t readEndingTime = getCurrentMicroseconds();
        int64_t deltaTime = readEndingTime - readStartingTime;
        double readTime = (double)(readEndingTime - readStartingTime) / readBufferCount * 0.001;
        double readSpeed = readBufferCount * randomDataSize*4 / (double)(deltaTime*1e-6); 
        printf("%s. Average read speed(MB/s): %f\n", api->name, readSpeed*0.000001);
    }

    api->close(file);
}

int main(int argc, const char *argv[])
{
    initializeClock();
    setRandomNumberSeed(42);

    // Generate the small data buffer.
    smallRandomData = malloc(sizeof(uint32_t)*smallRandomDataSize);
    smallReadDataBuffer = malloc(sizeof(uint32_t)*smallRandomDataSize);
    for(int i = 0; i < smallRandomDataSize; ++i)
        smallRandomData[i] = nextRandomNumber();

    // Generate random data buffer.
    randomData = malloc(sizeof(uint32_t)*randomDataSize);
    readDataBuffer = malloc(sizeof(uint32_t)*randomDataSize);
    for(int i = 0; i < randomDataSize; ++i)
        randomData[i] = nextRandomNumber();

    benchmarkFileApi(&stdio_file_api);
#ifdef _WIN32
    benchmarkFileApi(&win32_file_api);
#else
    benchmarkFileApi(&posix_file_api);
    benchmarkFileApi(&posix_explicit_offset_file_api);
    benchmarkFileApi(&posix_mmap_file_api);
#endif
    
    free(randomData);
    free(readDataBuffer);
    free(smallRandomData);
    free(smallReadDataBuffer);
    return 0;
}
