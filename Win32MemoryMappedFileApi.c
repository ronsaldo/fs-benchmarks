#include "Common.h"

#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct win32_mmaped_file_s {
    file_api_file_t super;
    HANDLE handle;
    HANDLE fileMappingHandle;
    uint8_t *fileMapping;
    size_t fileMappingSize;
    size_t offset;
} win32_mmaped_file_t;

static file_api_file_t * win32_mmaped_file_open(const char *fileName, bool writeMode)
{
    fprintf(stderr, "MMapped file requires an initial file size\n");
    abort();
}

static file_api_file_t * win32_mmaped_file_openWriteWithSize(const char *fileName, size_t fileSize)
{
    HANDLE fileHandle = CreateFileA(fileName,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if(!fileHandle)
    {
        perror("Failed to open file.");
        return NULL;
    }

    win32_mmaped_file_t *win32File = calloc(1, sizeof(win32_mmaped_file_t));
    win32File->handle = fileHandle;

    win32File->fileMappingHandle = CreateFileMappingA(win32File->handle, NULL,
            PAGE_READWRITE,
            (DWORD)(fileSize >> 32),  (DWORD)fileSize,
            NULL);
    if(!win32File->fileMappingHandle)
    {
        perror("Failed to create file mapping object.");
        CloseHandle(fileHandle);
        free(win32File);
        return NULL;
    }

    win32File->fileMappingSize = fileSize;
    win32File->fileMapping = MapViewOfFile(win32File->fileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, win32File->fileMappingSize);
    if(!win32File->fileMapping)
    {
        perror("Failed to create file mapping view.");
        CloseHandle(fileHandle);
        free(win32File);
        return NULL;
    }

    return &win32File->super;
}

static void win32_mmaped_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    win32_mmaped_file_t *win32File = (win32_mmaped_file_t*)file;
    CloseHandle(win32File->fileMappingHandle);
    CloseHandle(win32File->handle);
    free(win32File);
}

static void win32_mmaped_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    win32_mmaped_file_t *win32File = (win32_mmaped_file_t*)file;
    win32File->offset = offset;
}

static int64_t win32_mmaped_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    win32_mmaped_file_t *win32File = (win32_mmaped_file_t*)file;
    return win32File->offset;
}

static int64_t win32_mmaped_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;

    win32_mmaped_file_t *win32File = (win32_mmaped_file_t*)file;
    size_t remainingBytes = win32File->fileMappingSize - win32File->offset;
    size_t readSize = bufferSize;
    if(remainingBytes < readSize)
        readSize = remainingBytes;

    memcpy(buffer, win32File->fileMapping + win32File->offset, readSize);
    win32File->offset += readSize;
    return readSize;
}

static int64_t win32_mmaped_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    win32_mmaped_file_t *win32File = (win32_mmaped_file_t*)file;
    size_t remainingBytes = win32File->fileMappingSize - win32File->offset;
    size_t writeSize = bufferSize;
    if(remainingBytes < writeSize)
        writeSize = remainingBytes;

    memcpy(win32File->fileMapping + win32File->offset, data, writeSize);
    win32File->offset += writeSize;
    return writeSize;
}

file_api_t win32_mmaped_file_api = {
    .name = "Win32 Memory Mapped",
    .open = win32_mmaped_file_open,
    .openWriteWithSize = win32_mmaped_file_openWriteWithSize,
    .close = win32_mmaped_file_close,
    .seek = win32_mmaped_file_seek,
    .tell = win32_mmaped_file_tell,
    .read = win32_mmaped_file_read,
    .write = win32_mmaped_file_write
};

#endif
