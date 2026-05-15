#pragma once
#include "NetworkThread/NetworkThread.hpp"
#include "TaskThread/TaskThread.hpp"


class Overseer {
public:
    Overseer(int wakeupFd);

    void startShutdown();


    void networkLoop();

    void taskLoop();




private:




    TaskManager taskManager;
    NetworkThread networkManager;

};




