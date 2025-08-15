#include "Common.h"

#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>

typedef struct win32_overlapped_file_s {
    file_api_file_t super;
    HANDLE handle;
    size_t offset;
} win32_overlapped_file_t;

static file_api_file_t * win32_overlapped_file_open(const char *fileName, bool writeMode)
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

    win32_overlapped_file_t *win32File = calloc(1, sizeof(win32_overlapped_file_t));
    win32File->handle = fileHandle;
    return &win32File->super;
}

static file_api_file_t * win32_overlapped_file_openWriteWithSize(const char *fileName, size_t fileSize)
{
    win32_overlapped_file_t *win32File = (win32_overlapped_file_t *)win32_overlapped_file_open(fileName, true);
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

static void win32_overlapped_file_close(file_api_file_t *file)
{
    if(!file)
        return;

    win32_overlapped_file_t *win32File = (win32_overlapped_file_t*)file;
    CloseHandle(win32File->handle);
    free(win32File);
}

static void win32_overlapped_file_seek(file_api_file_t *file, size_t offset)
{
    if(!file)
        return;

    win32_overlapped_file_t *win32File = (win32_overlapped_file_t*)file;
    win32File->offset = offset;
}

static int64_t win32_overlapped_file_tell(file_api_file_t *file)
{
    if(!file)
        return 0;

    win32_overlapped_file_t *win32File = (win32_overlapped_file_t*)file;
    return win32File->offset;
}

static int64_t win32_overlapped_file_read(file_api_file_t *file, size_t bufferSize, void* buffer)
{
    if(!file)
        return 0;
    win32_overlapped_file_t *win32File = (win32_overlapped_file_t*)file;

    OVERLAPPED overlapped = {0};
    overlapped.Offset = (DWORD)win32File->offset;
    overlapped.OffsetHigh = (DWORD)(win32File->offset >> 32);

    DWORD readBytes = 0;
    if(ReadFile(win32File->handle, buffer, (DWORD)bufferSize, &readBytes, &overlapped))
        win32File->offset += readBytes;
    return readBytes;
}

static int64_t win32_overlapped_file_write(file_api_file_t *file, size_t bufferSize, const void* data)
{
    if(!file)
        return 0;

    win32_overlapped_file_t *win32File = (win32_overlapped_file_t*)file;
    OVERLAPPED overlapped = {0};
    overlapped.Offset = (DWORD)win32File->offset;
    overlapped.OffsetHigh = (DWORD)(win32File->offset >> 32);

    DWORD writtenBytes = 0;
    if(WriteFile(win32File->handle, data, (DWORD)bufferSize, &writtenBytes, NULL))
        win32File->offset += writtenBytes;
    return writtenBytes;
}

file_api_t win32_overlapped_file_api = {
    .name = "Win32 Overlapped",
    .open = win32_overlapped_file_open,
    .openWriteWithSize = win32_overlapped_file_openWriteWithSize,
    .close = win32_overlapped_file_close,
    .seek = win32_overlapped_file_seek,
    .tell = win32_overlapped_file_tell,
    .read = win32_overlapped_file_read,
    .write = win32_overlapped_file_write
};

#endif