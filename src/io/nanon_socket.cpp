
#include "nanon_socket.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


NanonSocket::NanonSocket()
{
    socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cout << "ERROR: Could not connect to socket" << std::endl;
    }
}


NanonSocket::~NanonSocket()
{

}


bool NanonSocket::writeAll(const void* data, size_t size)
{
    const char* ptr = static_cast<const char*>(data);

    while (size > 0)
    {
        ssize_t written = write(socketFd, ptr, size);

        if (written <= 0) {
            return false;
        }

        ptr += written;
        size -= written;
    }

    return true;
}


bool NanonSocket::sendJson(const std::string& jsonStr)
{
    uint32_t len = jsonStr.size();
    uint32_t netLen = htonl(len);

    if (!writeAll(&netLen, sizeof(netLen))) {
        return false;
    }

    return writeAll(jsonStr.data(), jsonStr.size());
}


bool NanonSocket::readAll(void* data, size_t size)
{
    char* ptr = static_cast<char*>(data);

    while (size > 0)
    {
        ssize_t n = recv(socketFd, ptr, size, 0);

        if (n <= 0)
            return false; // error or disconnected

        ptr  += n;
        size -= n;
    }

    return true;
}