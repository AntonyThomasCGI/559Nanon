#pragma once

#include <stddef.h>
#include <string>


static const char* SOCKET_PATH = "/tmp/nanon.sock";


class NanonSocket
{
public:
    NanonSocket();
    virtual ~NanonSocket();

    bool sendJson(const std::string& jsonStr);
    bool writeAll(const void* data, size_t size);
    bool readAll(void* data, size_t size);

private:

    int socketFd;
};
