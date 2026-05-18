#pragma once
#include <condition_variable>

#include "includes.hpp"
#include "EpollManager/EpollManager.hpp"
#include "Client.hpp"
#include <map>
#include <memory>


#include "TaskIncoming.hpp"


class NetworkThread {
public:

    NetworkThread(int wakeupFd);



    void assignTasks(std::vector<TaskOutgoing> &&tasks);


    void shutdown();




    std::optional<std::vector<TaskIncoming>> loopOver();



private:
    void wakeup();

    std::optional<InformationUnit> handleClient(std::shared_ptr<Client> client, const epoll_event &ev);

    void loopOverTasks(const std::vector<TaskOutgoing> &tasks);


    void addClient();


    EpollManager epoll_manager;
    std::vector<std::shared_ptr<Client>> clients;


    std::mutex tasksMut;
    std::vector<TaskOutgoing> tasks;


    std::mutex shutdownMut;
    bool shutdownFlag = false;

    int listenFd;
    int wakeupFd;

};




