#include "Connection.hpp"
#include "Parsing/parsing.hpp"
#include <cstring>

Connection::Connection(int socketFd): sock(socketFd) {}

Connection::~Connection() {
    close(sock);
}


Connection::SockResult Connection::performRead() {
    int test = recv(sock, readBuff.data(), sizeof(std::uint32_t), 0);
    if (test == -1) {
        return FAILED;
    }
    const Byte *ptr = (Byte *)readBuff.data();
    std::uint32_t toRead  = Parsing::parse_int<std::uint32_t>(ptr);

    test = recv(sock, readBuff.data(), toRead, 0);
    if (test == -1) {
        return FAILED;
    }
    return SUCCCESFUL;
}

const Buffer &Connection::relinquish_buff() const {
    return readBuff;
}


bool Connection::performWrite(const std::vector<Byte> &bytes) {
    auto test = send(sock, bytes.data(), bytes.size(), 0);
    if (test != bytes.size()) {
        return false;
    }
    return true;
}

int Connection::getSocket() {
    return sock;
}



