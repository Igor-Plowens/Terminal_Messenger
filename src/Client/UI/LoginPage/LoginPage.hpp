#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"



class LoginPage {
public:
    LoginPage(Queue &eventQueue);



    ftxui::Component getRenderer();


    void setFeedback(const std::string &text);



private:

    Queue &eventQueue;


    ftxui::Component container;

    ftxui::Component inputUsername;
    ftxui::Component inputPassword;
    ftxui::Component sendButton;

    ftxui::Component backToStartingButton;

    std::string username;
    std::string password;

    std::string feedback = "Enter data";
    ftxui::Component renderer;
};

