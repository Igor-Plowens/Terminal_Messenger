#pragma once

#include "UI/DmPage/DmPage.hpp"
#include "UI/LoadingPage/LoadingPage.hpp"
#include "UI/LoginPage/LoginPage.hpp"
#include "UI/MenuPage/MenuPage.hpp"
#include "UI/RegisterPage/RegisterPage.hpp"
#include "UI/StartingPage/StartingPage.hpp"

#include "Connection/Connection.hpp"
#include "InformationUnit/InformationUnit.hpp"

#include <fstream>


class Overseer {
public:
    Overseer(int sockFd, ftxui::ScreenInteractive &screen);



    //void networkReader();
    //void networkWriter(); todo: remove completely once shifted to networker

    void handleQueue();


    ftxui::Component getTab();



private:

    void reactToInfo(const InformationUnit &unit);



    ftxui::ScreenInteractive &screen;
    Queue eventQueue;


    bool shutdownFlag = false;




    //Connection connection;
    //std::deque<InformationUnit> writeQueue;
    //std::mutex writeQueueMut;
    //std::condition_variable writeQueueCV;



};
