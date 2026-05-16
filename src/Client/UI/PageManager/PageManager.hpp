#pragma once
#include "UI/LoadingPage/LoadingPage.hpp"
#include "UI/LoginPage/LoginPage.hpp"
#include "UI/RegisterPage/RegisterPage.hpp"
#include "UI/StartingPage/StartingPage.hpp"
#include "UI/MenuPage/MenuPage.hpp"



class PageManager {
public:


    PageManager(Queue &queue);

    ftxui::Component getTab();


    void setSelector(PageType val);

    PageType getLoadingsMethodOfEntry();

    void forwardFeedbackString(const std::string &str);



private:

    int selector = 0;

    StartingPage startingPage;
    RegisterPage registerPage;
    LoginPage loginPage;
    MenuPage menuPage;
    LoadingPage loadingPage;


    ftxui::Component container;
};




