#include "EpollManager.hpp"
#include <algorithm>
#include <stdexcept>


EpollManager::EpollManager() {
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        throw std::runtime_error("epoll_create1 failed");
    }
}

EpollManager::~EpollManager(){
    close(epollFd);
}





void EpollManager::addEvent(int sock) {
    epoll_event ev{0};
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, sock, &ev);
}


void EpollManager::removeEvent(int sock){

    epoll_ctl(epollFd, EPOLL_CTL_DEL, sock, nullptr);
}

void EpollManager::giveEpollout(int sock){
    epoll_event ev{0};
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sock;
    epoll_ctl(epollFd, EPOLL_CTL_MOD, sock, &ev);
}


void EpollManager::removeEpollout(int sock){
    epoll_event ev{0};
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    epoll_ctl(epollFd, EPOLL_CTL_MOD, sock, &ev);
}



ReadyList EpollManager::wait(){
    int res = epoll_wait(epollFd, buff, maxEvents, -1);
    if (res == -1) {
        throw std::runtime_error("epoll_wait failed");
    }
    return {buff, static_cast<Size>(res)};
}


