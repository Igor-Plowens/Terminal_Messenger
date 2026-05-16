#pragma once

#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"

struct DmMessage {

    bool isMine;
    std::string content;
    ID_t messageId;
};


class DmPage {
public:
    DmPage(Queue &eventQueue);

    ftxui::Component getRenderer();

    void setRecipient(const std::string& recipient);

    std::string getRecipient() const;

    void addMessage(DmMessage &&message);



    void incrementOffset();
    void decrementOffset();





private:
    Queue &eventQueue;



    ftxui::Component messageInput;
    std::string messageStr;

    ftxui::Component sendButton;

    ftxui::Component backToMenuButton;




    std::string recipient;


    ftxui::Component container;
    ftxui::Component renderer;


    std::mutex dmMessageMutex;
    std::deque<DmMessage> dmMessagesCache;
    int offset = 0;

};