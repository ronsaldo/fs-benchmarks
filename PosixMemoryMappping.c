#include "Common.h"

#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct posix_mmap_file_s {
    file_api_file_t super;
    int fd;
    size_t offset;
    uint8_t *memoryMap;
    size_t memoryMapSize;
} posix_mmap_file_t;


static file_api_file_t * posix_mmap_file_open(const char *fileName, bool writeMode)
{
    fprintf(stderr,"Posix MMap requires a explicit size.\n");
    abort();
}

static file_api_file_t * posix_mmap_file_openWriteWithSize(const char *fileName, size_t fileSize)
{
    int openMode = O_RDWR |O_CREAT | O_TRUNC;
    int fd = open(fileName, openMode, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fd < 0)
    {
        perror("Failed to open file.");
        return NULL;
    }
    ftruncate(fd, fileSize);

    uint8_t *memoryMap = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(memoryMap == MAP_FAILED)
    {
        perror("Memory map failed.");
        close(fd);
        return NULL;
    }

    posix_mmap_file_t *posixFile = calloc(1, sizeof(posix_mmap_file_t));
    posixFile->fd = fd;
    posixFile->memoryMap = memoryMap;
    posixFile->memoryMapSize = fileSize;
    return &posixFile->super;
}

static void posix_mmap_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    posix_mmap_file_t *posixFile = (posix_mmap_file_t*)file;
    munmap(posixFile->memoryMap, posixFile->memoryMapSize);
    close(posixFile->fd);
    free(posixFile);
}

static void posix_mmap_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    posix_mmap_file_t *posixFile = (posix_mmap_file_t*)file;
    posixFile->offset = offset;
}

static int64_t posix_mmap_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    posix_mmap_file_t *posixFile = (posix_mmap_file_t*)file;
    return posixFile->offset;
}

static int64_t posix_mmap_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;

    posix_mmap_file_t *posixFile = (posix_mmap_file_t*)file;
    size_t remainingBytes = posixFile->memoryMapSize - posixFile->offset;
    size_t readSize = bufferSize;
    if(remainingBytes < readSize)
        readSize = remainingBytes;

    memcpy(buffer, posixFile->memoryMap + posixFile->offset, readSize);
    posixFile->offset += readSize;
    return readSize;
}

static int64_t posix_mmap_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    posix_mmap_file_t *posixFile = (posix_mmap_file_t*)file;
    size_t remainingBytes = posixFile->memoryMapSize - posixFile->offset;
    size_t writeSize = bufferSize;
    if(remainingBytes < writeSize)
        writeSize = remainingBytes;

    memcpy(posixFile->memoryMap + posixFile->offset, data, writeSize);
    posixFile->offset += writeSize;
    return writeSize;
}

file_api_t posix_mmap_file_api = {
    .name = "Posix MMap",
    .open = posix_mmap_file_open,
    .openWriteWithSize = posix_mmap_file_openWriteWithSize,
    .close = posix_mmap_file_close,
    .seek = posix_mmap_file_seek,
    .tell = posix_mmap_file_tell,
    .read = posix_mmap_file_read,
    .write = posix_mmap_file_write
};

#endif // _WIN32