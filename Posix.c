#include "Common.h"

#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct posix_file_s {
    file_api_file_t super;
    int fd;
} posix_file_t;


static file_api_file_t * posix_file_open(const char *fileName, bool writeMode)
{
    int openMode = writeMode ? O_RDWR |O_CREAT | O_TRUNC : O_RDONLY;
    int fd = open(fileName, openMode, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fd < 0)
    {
        perror("Failed to open file.");
        return NULL;
    }

    posix_file_t *posixFile = calloc(1, sizeof(posix_file_t));
    posixFile->fd = fd;
    return &posixFile->super;
}

static file_api_file_t * posix_file_openWriteWithSize(const char *fileName, size_t fileSize)
{
    posix_file_t *posixFile = (posix_file_t *)posix_file_open(fileName, true);
    if(posixFile)
        ftruncate(posixFile->fd, fileSize);
    return &posixFile->super;
}

static void posix_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    posix_file_t *posixFile = (posix_file_t*)file;
    close(posixFile->fd);
    free(posixFile);
}

static void posix_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    posix_file_t *posixFile = (posix_file_t*)file;
    lseek(posixFile->fd, offset, SEEK_SET);
}

static int64_t posix_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    posix_file_t *posixFile = (posix_file_t*)file;
    return lseek(posixFile->fd, 0, SEEK_CUR);
}

static int64_t posix_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;

    posix_file_t *posixFile = (posix_file_t*)file;
    return (int64_t)read(posixFile->fd, buffer, bufferSize);
}

static int64_t posix_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    posix_file_t *posixFile = (posix_file_t*)file;
    return (int64_t)write(posixFile->fd, data, bufferSize);
}

file_api_t posix_file_api = {
    .name = "Posix",
    .open = posix_file_open,
    .openWriteWithSize = posix_file_openWriteWithSize,
    .close = posix_file_close,
    .seek = posix_file_seek,
    .tell = posix_file_tell,
    .read = posix_file_read,
    .write = posix_file_write
};

#endif // _WIN32