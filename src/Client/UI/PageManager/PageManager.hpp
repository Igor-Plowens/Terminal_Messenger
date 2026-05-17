#pragma once
#include "UI/DmPage/DmPage.hpp"
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

    PageType getPageType();

    PageType getLoadingsMethodOfEntry();

    void forwardFeedbackString(const std::string &str);


    void forwardDmMessage(DmMessage &&dmMessage);

    void incrementDmCache();
    void decrementDmCache();

    std::string getDmRecipient();

    std::string nickname;



private:

    int selector = 0;

    StartingPage startingPage;
    RegisterPage registerPage;
    LoginPage loginPage;
    LoadingPage loadingPage;
    MenuPage menuPage;
    DmPage dmPage;


    ftxui::Component container;

};




