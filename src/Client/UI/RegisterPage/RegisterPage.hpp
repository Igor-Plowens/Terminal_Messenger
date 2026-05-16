#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"



class RegisterPage {
public:
    RegisterPage(Queue &eventQueue);


    //ftxui::Component getContainer();

    ftxui::Component getRenderer();


    void setFeedback(const std::string &text);


private:

    Queue &eventQueue;

    ftxui::Component inputUsername;
    ftxui::Component inputPassword;
    ftxui::Component sendButton;

    ftxui::Component backToStartingButton;

    std::string username;
    std::string password;

    ftxui::Component renderer;
    std::string feedback = "Enter data";

    ftxui::Component container;

};
