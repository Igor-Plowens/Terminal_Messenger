#include "Connection.hpp"
#include "Parsing/parsing.hpp"
#include <cerrno>
#include <stdexcept>
#include <cstring>


Connection::Connection(int socket_fd) noexcept : socket(socket_fd) {}




Connection::SockResult Connection::perform_read() noexcept{
    if (!lenReady){
        while (readSoFar != sizeof(std::uint32_t)){
            ssize_t test = recv(socket, reinterpret_cast<void *>(readBuff.data() + readSoFar), sizeof(std::uint32_t) - readSoFar, 0);

            if (test == 0) return DISCONNECT;
            if (test < 0){
                if (errno == EAGAIN || errno == EWOULDBLOCK){
                    return INCOMPLETE;
                }
                return DISCONNECT;
            }
            readSoFar += test;
        }
        const Byte *buff = readBuff.data();
        toRead = Parsing::parse_int<std::uint32_t>(buff);
        readSoFar = 0;
        lenReady = true;
    }
    while (readSoFar != toRead){
        ssize_t test = recv(socket, reinterpret_cast<void *>(readBuff.data() + readSoFar), toRead - readSoFar, 0);

        if (test == 0) return DISCONNECT;
        if (test < 0){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                return INCOMPLETE;
            }
            return DISCONNECT;
        }
        readSoFar += test;
    }
    readSoFar = toRead = 0;
    lenReady = false;
    return COMPLETE;
}

const Buffer & Connection::relinquish_buffer(){
    return readBuff;
}

void Connection::clearBuffer() {
    std::memset(readBuff.data(), 0, readBuff.size());
}


Connection::SockResult Connection::perform_write() noexcept{
    while (writtenSoFar != toWrite){
        ssize_t test = send(socket, reinterpret_cast<void *>(writeBuff.data() + writtenSoFar),
                    toWrite - writtenSoFar, 0);
        if (test == 0){
            return DISCONNECT;
        }
        if (test < 0){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                return INCOMPLETE;
            }
            return DISCONNECT;
        }
        writtenSoFar += test;
    }
    writtenSoFar = 0;
    std::lock_guard lck(writesQueueMut);
    if (!writesQueue.empty()){
        replace_write_buff();
        return INCOMPLETE;
    }
    toWrite = 0;
    isWriteBuffEmpty = true;
    return COMPLETE;

}

void Connection::replace_write_buff(){
    std::memcpy(writeBuff.data(), writesQueue[0].data(), writesQueue[0].size());
    toWrite = writesQueue[0].size();
    writesQueue.pop_front();
    isWriteBuffEmpty = false;
}

void Connection::assign_write(std::vector<Byte> &&vec) noexcept{
    std::lock_guard<std::mutex> lck(writesQueueMut);
    writesQueue.push_back(std::move(vec));
    if (isWriteBuffEmpty){
        replace_write_buff();
    }
}

