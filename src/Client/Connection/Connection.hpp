#pragma once
#include "includes.hpp"
#include <vector>

class Connection {
public:
    enum SockResult {
        SUCCCESFUL,
        FAILED
    };
    explicit Connection(int socketFd);
    ~Connection();

    SockResult performRead();
    const Buffer &relinquish_buff() const;

    bool performWrite(const std::vector<Byte> &bytes);

    int getSocket();

private:
    int sock;

    Buffer readBuff;
};