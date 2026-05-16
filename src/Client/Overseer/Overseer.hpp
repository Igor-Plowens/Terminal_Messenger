#pragma once

#include "UI/PageManager/PageManager.hpp"
#include "Connection/Connection.hpp"
#include "InformationUnit/InformationUnit.hpp"

#include <fstream>


class Overseer {
public:
    Overseer(int sockFd, ftxui::ScreenInteractive &screen);

    void setShutdown();

    void networkReader();
    void networkWriter();

    void handleQueue();


    ftxui::Component getTab();



private:

    void sendInfo(const InformationUnit &unit);

    void reactToInfo(const InformationUnit &unit);



    ftxui::ScreenInteractive &screen;
    Queue eventQueue;



    Connection connection;
    PageManager pageManager;




    std::deque<InformationUnit> writeQueue;
    std::mutex writeQueueMut;
    std::condition_variable writeQueueCV;


    std::ofstream myFile{"DEBUG_THIS.txt"};

    bool shutdownFlag = false;
};
