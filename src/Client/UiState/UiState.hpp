#pragma once


#include "UI/DmPage/DmPage.hpp"
#include "UI/LoadingPage/LoadingPage.hpp"
#include "UI/LoginPage/LoginPage.hpp"
#include "UI/MenuPage/MenuPage.hpp"
#include "UI/RegisterPage/RegisterPage.hpp"
#include "UI/StartingPage/StartingPage.hpp"

#include "Queue/Queue.hpp"


struct UiState {
    UiState(ftxui::ScreenInteractive &screen, Queue &queue);

    ftxui::Component getFinalContainer();


    ftxui::ScreenInteractive &screen;


    ftxui::Component container;

    LoadingPage loadingPage;

    StartingPage startingPage;
    LoginPage loginPage;
    RegisterPage registerPage;
    MenuPage menuPage;
    DmPage dmPage;

    int selector = PageType::STARTING_PAGE;


    std::string myNickname;
    ID_t myId;


};