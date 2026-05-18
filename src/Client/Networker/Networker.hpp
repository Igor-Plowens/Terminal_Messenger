#pragma once
#include "includes.hpp"
#include "Connection/Connection.hpp"
#include <deque>
#include "InformationUnit/InformationUnit.hpp"
#include <mutex>
#include <condition_variable>

#include "Queue/Queue.hpp"

class Networker {
public:
    Networker(int sockfd, Queue &eventQueue);



    void setShutdown();
    void networkReadingThread();
    void networkWritingThread();

    void queueWrite(InformationUnit unit);


private:
    Task convertInformationToTask(InformationUnit unit);


    Queue &eventQueue;

    Connection connection;
    std::deque<InformationUnit> writeQueue;
    std::mutex writeQueueMut;
    std::condition_variable writeQueueCV;

    bool shutdownFlag = false;
};



