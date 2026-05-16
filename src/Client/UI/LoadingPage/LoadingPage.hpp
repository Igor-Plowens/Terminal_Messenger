#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"


class LoadingPage {
public:
    LoadingPage(Queue &eventQueue);

    ftxui::Component getRenderer();

    void setMethodOfEntry(PageType method);
    PageType getMethodOfEntry();


private:

    PageType methodOfEntry;

    ftxui::Component renderer;

    Queue &eventQueue;
};