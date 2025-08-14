#ifndef FS_BENCHMARKS_COMMON_H
#define FS_BENCHMARKS_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void initializeClock(void);
int64_t getCurrentMicroseconds(void);
void setRandomNumberSeed(uint32_t seed);
uint32_t nextRandomNumber();

typedef struct file_api_file_s
{
} file_api_file_t;

typedef struct file_api_s
{
    const char *name;
    file_api_file_t *(*open)(const char *fileName, bool writeMode);
    file_api_file_t *(*openWriteWithSize)(const char *fileName, size_t fileSize);
    void (*close)(file_api_file_t *file);
    void (*seek)(file_api_file_t *file, size_t offset);
    int64_t (*tell)(file_api_file_t *file);
    int64_t (*read)(file_api_file_t *file, size_t bufferSize, void* buffer);
    int64_t (*write)(file_api_file_t *file, size_t bufferSize, const void* data);
} file_api_t;

extern file_api_t stdio_file_api;

#ifndef _WIN32
extern file_api_t posix_file_api;
extern file_api_t posix_explicit_offset_file_api;
#endif

#endif // FS_BENCHMARKS_COMMON_H