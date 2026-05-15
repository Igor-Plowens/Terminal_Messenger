#pragma once
#include <tuple>
#include "includes.hpp"
#include <list>

struct ReadyList{
    ReadyList(epoll_event *buff, std::uint32_t count): buffer(buff), count(count) {}
    epoll_event *buffer;
    Size count;
};




constexpr int maxEvents = 1024;
class EpollManager{
public:
    EpollManager();
    ~EpollManager();




    ReadyList wait();


    void addEvent(int sock);


    void removeEvent(int sock);




    void giveEpollout(int sock);
    void removeEpollout(int sock);


private:
    int epollFd;
    epoll_event buff[maxEvents];
};