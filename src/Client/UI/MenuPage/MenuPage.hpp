#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"

class MenuPage {
public:
    MenuPage(Queue &eventQueue);


    ftxui::Component getRenderer();


    std::string getChosenRecipient();

    void setFeedback(const std::string &feedback);


private:
    Queue &eventQueue;

    ftxui::Component dmDestinationInput;
    std::string dmDestination;
    ftxui::Component enterDmButton;

    ftxui::Component leaveButton;

    ftxui::Component container;

    ftxui::Component renderer;

    std::string feedback;

};