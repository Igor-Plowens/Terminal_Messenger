#pragma once
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), recv(), send()
#include <netinet/in.h>   // sockaddr_in, sockaddr_in6, IPPROTO_TCP, htons()
#include <arpa/inet.h>    // inet_pton(), inet_ntop()
#include <unistd.h>       // close(), read(), write()

#include <sys/epoll.h>    // epoll_create1(), epoll_ctl(), epoll_wait()
#include <fcntl.h>        // fcntl(), O_NONBLOCK
#include <cerrno>        // errno, EAGAIN, EWOULDBLOCK, EINTR
#include <netdb.h>        // getaddrinfo(), freeaddrinfo()
#include <sys/eventfd.h>


#include <array>
#include <cstdint>


namespace ProtConstants {
    constexpr int BUFFER_SIZE = 15000;
}

using Byte = std::uint8_t;
using Buffer = std::array<Byte, ProtConstants::BUFFER_SIZE>;
using ID_t = std::uint64_t;
using StrLen = std::uint16_t;
using Size = std::uint32_t;
using Count = std::uint16_t;

