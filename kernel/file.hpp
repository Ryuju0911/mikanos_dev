#pragma once

#include <cstddef>

class FileDescriptor {
  public:
    virtual ~FileDescriptor() = default;
    virtual size_t Read(void *buf, size_t len) = 0;
    virtual size_t Write(const void *buf, size_t len) = 0;
    virtual size_t Size() const = 0;

    /** @brief Reads file content starting at the specified offset. */
    virtual size_t Load(void *buf, size_t len, size_t offset) = 0;
};

size_t PrintToFD(FileDescriptor &fd, const char *format, ...);
size_t ReadDelim(FileDescriptor &fd, char delim, char *buf, size_t len);