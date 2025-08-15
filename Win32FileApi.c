#include "Common.h"

#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>

typedef struct win32_file_s {
    file_api_file_t super;
    HANDLE handle;
} win32_file_t;

static file_api_file_t * win32_file_open(const char *fileName, bool writeMode)
{
    HANDLE fileHandle = CreateFileA(fileName,
            writeMode ? GENERIC_WRITE | GENERIC_READ : GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        writeMode ? CREATE_ALWAYS : OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if(!fileHandle)
    {
        perror("Failed to open file.");
        return NULL;
    }

    win32_file_t *win32File = calloc(1, sizeof(win32_file_t));
    win32File->handle = fileHandle;
    return &win32File->super;
}

static file_api_file_t * win32_file_openWriteWithSize(const char *fileName, size_t fileSize)
{
    win32_file_t *win32File = (win32_file_t *)win32_file_open(fileName, true);
    if(win32File)
    {
        LARGE_INTEGER fileSizeInteger;
        fileSizeInteger.QuadPart = fileSize;

        SetFilePointerEx(win32File->handle, fileSizeInteger, NULL, FILE_BEGIN);
        SetEndOfFile(win32File->handle);
        SetFilePointer(win32File->handle, 0, NULL, FILE_BEGIN);
    }
        
    return &win32File->super;
}

static void win32_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    win32_file_t *win32File = (win32_file_t*)file;
    CloseHandle(win32File->handle);
    free(win32File);
}

static void win32_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    win32_file_t *win32File = (win32_file_t*)file;
    LARGE_INTEGER fileOffsetInteger;
    fileOffsetInteger.QuadPart = offset;
    SetFilePointerEx(win32File->handle, fileOffsetInteger, NULL, FILE_BEGIN);
}

static int64_t win32_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    win32_file_t *win32File = (win32_file_t*)file;
    LARGE_INTEGER zeroOffset;
    zeroOffset.QuadPart = 0;

    LARGE_INTEGER currentOffset;    
    SetFilePointerEx(win32File->handle, zeroOffset, &currentOffset, FILE_CURRENT);
    return currentOffset.QuadPart;
}

static int64_t win32_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;

    win32_file_t *win32File = (win32_file_t*)file;
    DWORD readBytes = 0;
    ReadFile(win32File->handle, buffer, (DWORD)bufferSize, &readBytes, NULL);
    return readBytes;
}

static int64_t win32_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    win32_file_t *win32File = (win32_file_t*)file;
    DWORD writtenBytes = 0;
    WriteFile(win32File->handle, data, (DWORD)bufferSize, &writtenBytes, NULL);
    return writtenBytes;
}

file_api_t win32_file_api = {
    .name = "Win32",
    .open = win32_file_open,
    .openWriteWithSize = win32_file_openWriteWithSize,
    .close = win32_file_close,
    .seek = win32_file_seek,
    .tell = win32_file_tell,
    .read = win32_file_read,
    .write = win32_file_write
};

#endif