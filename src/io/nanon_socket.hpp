#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <stddef.h>
#include <string>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


namespace nanon {


int connectToSocket(std::string socketPath)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cout << "ERROR: Could not connect to socket" << std::endl;
    }
    return fd;
};


bool writeAll(int fd, const void* data, size_t size)
{
    const char* ptr = static_cast<const char*>(data);

    while (size > 0)
    {
        ssize_t written = write(fd, ptr, size);

        if (written <= 0) {
            return false;
        }

        ptr += written;
        size -= written;
    }

    return true;
};


bool sendJson(int fd, const std::string& jsonStr)
{
    uint32_t len = jsonStr.size();
    uint32_t netLen = htonl(len);

    if (!writeAll(fd, &netLen, sizeof(netLen))) {
        return false;
    }

    return writeAll(fd, jsonStr.data(), jsonStr.size());
};


bool readAll(int fd, void* data, size_t size)
{
    char* ptr = static_cast<char*>(data);

    while (size > 0)
    {
        ssize_t n = recv(fd, ptr, size, 0);

        if (n <= 0)
            return false; // error or disconnected

        ptr  += n;
        size -= n;
    }

    return true;
};

};  // namespace nanon
