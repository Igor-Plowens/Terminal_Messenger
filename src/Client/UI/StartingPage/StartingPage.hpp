#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"


class StartingPage {
public:
    StartingPage(Queue &eventQueue);


    ftxui::Component getContainer();




private:

    Queue &eventQueue;

    ftxui::Component goToLoginPageButton;
    ftxui::Component goToRegisterPageButton;

    ftxui::Component leaveButton;

    ftxui::Component container;
};
