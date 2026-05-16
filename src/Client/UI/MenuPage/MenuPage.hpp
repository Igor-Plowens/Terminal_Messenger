#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"

class MenuPage {
public:
    MenuPage(Queue &eventQueue);

    ftxui::Component getContainer();


private:
    Queue &eventQueue;

    ftxui::Component leaveButton;

    ftxui::Component container;
};