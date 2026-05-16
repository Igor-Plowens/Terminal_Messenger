#pragma once
#include "includesFTXUI.hpp"
#include "Queue/Queue.hpp"


class LoadingPage {
public:
    LoadingPage(Queue &eventQueue);

    ftxui::Component getRenderer();


private:

    ftxui::Component renderer;

    Queue &eventQueue;
};