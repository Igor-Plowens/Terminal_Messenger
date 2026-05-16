#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"

class MenuPage {
public:
    MenuPage(Queue &eventQueue);

    ftxui::Component getContainer();

    std::string getChosenRecipient();


private:
    Queue &eventQueue;

    ftxui::Component dmDestinationInput;
    std::string dmDestination;
    ftxui::Component enterDmButton;

    ftxui::Component leaveButton;

    ftxui::Component container;

};