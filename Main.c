#include "Common.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct stdio_file_s {
    file_api_file_t super;
    FILE *handle;
} stdio_file_t;

file_api_file_t * stdio_file_open(const char *fileName, bool writeMode)
{
    FILE *handle = fopen(fileName, writeMode ? "wb" : "rb");
    if(!handle)
    {
        perror("Failed to open file");
        return NULL; 
    }

    stdio_file_t *file = calloc(1, sizeof(stdio_file_t));
    file->handle = handle;
    return &file->super;
}

void stdio_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    stdio_file_t *stdioFile = (stdio_file_t*)file;
    fclose(stdioFile->handle);
    free(stdioFile);
}

void stdio_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    stdio_file_t *stdioFile = (stdio_file_t*)file;
    fseek(stdioFile->handle, offset, SEEK_SET);
}

int64_t stdio_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    stdio_file_t *stdioFile = (stdio_file_t*)file;
    return ftell(stdioFile->handle);
}

int64_t stdio_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;

    stdio_file_t *stdioFile = (stdio_file_t*)file;
    return (int64_t)fread(buffer, 1, bufferSize, stdioFile->handle);
}
int64_t stdio_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    stdio_file_t *stdioFile = (stdio_file_t*)file;
    return (int64_t)fwrite(data, 1, bufferSize, stdioFile->handle);
}

file_api_t stdio_file_api = {
    .name = "Stdio",
    .open = stdio_file_open,
    .close = stdio_file_close,
    .seek = stdio_file_seek,
    .tell = stdio_file_tell,
    .read = stdio_file_read,
    .write = stdio_file_write
};

int main(int argc, const char *argv[])
{
    initializeClock();
    return 0;
}
