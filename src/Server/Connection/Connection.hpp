#pragma once
#include "includes.hpp"
#include <mutex>
#include <deque>
#include <vector>





class Connection{
public:
    enum SockResult{
        INCOMPLETE,
        DISCONNECT,
        COMPLETE,
    };

    explicit Connection(int socket) noexcept;
    ~Connection(){
        close(socket);
    }


    SockResult perform_read() noexcept;
    const Buffer &relinquish_buffer();
    void clearBuffer();

    void assign_write(std::vector<Byte> &&vec) noexcept;
    SockResult perform_write() noexcept;

    int get_sock() const{
        return socket;
    }

    bool is_write_buff_empty() const{
        return isWriteBuffEmpty;
    }



private:
    void replace_write_buff();


    Buffer readBuff{};
    Size readSoFar = 0;
    Size toRead = 0;
    bool lenReady = false;



    Buffer writeBuff{};
    std::uint32_t writtenSoFar = 0;
    std::uint32_t toWrite = 0;

    bool isWriteBuffEmpty = true;


    std::mutex writesQueueMut;
    std::deque<std::vector<Byte>> writesQueue;

    int socket;



};



