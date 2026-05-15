#pragma once
#include "includesFTXUI.hpp"

class MenuPage {
public:
    MenuPage();

    ftxui::Component getContainer();


private:
    ftxui::Component button;

    ftxui::Component container;
};