#pragma once
#include "UI/LoadingPage/LoadingPage.hpp"
#include "UI/LoginPage/LoginPage.hpp"
#include "UI/RegisterPage/RegisterPage.hpp"
#include "UI/StartingPage/StartingPage.hpp"
#include "UI/MenuPage/MenuPage.hpp"



class PageManager {
public:

    enum PageType {
        STARTING_PAGE = 0,
        REGISTER_PAGE = 1,
        LOGIN_PAGE = 2,
        LOADING_PAGE = 3,
        MENU = 4
    };

    PageManager(Queue &queue);

    ftxui::Component getTab();


    void setSelector(PageType val);



private:

    int selector = 0;

    StartingPage startingPage;
    RegisterPage registerPage;
    LoginPage loginPage;
    MenuPage menuPage;
    LoadingPage loadingPage;


    ftxui::Component container;
};




