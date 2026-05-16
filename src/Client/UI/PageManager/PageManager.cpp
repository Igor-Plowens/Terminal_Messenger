#include "PageManager.hpp"


PageManager::PageManager(Queue &queue): startingPage(queue), registerPage(queue), loginPage(queue), loadingPage(queue), menuPage(queue) {
    container = ftxui::Container::Tab({startingPage.getContainer(), registerPage.getRenderer(), loginPage.getRenderer(), loadingPage.getRenderer(), menuPage.getContainer()}, &selector);
}


ftxui::Component PageManager::getTab() {
    return container;
}

void PageManager::setSelector(PageType val) {
    if (val == LOADING_PAGE) {
        loadingPage.setMethodOfEntry(static_cast<PageType>(selector));
    }
    selector = val;
}

PageType PageManager::getLoadingsMethodOfEntry() {
    return loadingPage.getMethodOfEntry();
}

void PageManager::forwardFeedbackString(const std::string &str) {
    if (loadingPage.getMethodOfEntry() == LOGIN_PAGE) {
        loginPage.setFeedback(str);
    }
    else {
        registerPage.setFeedback(str);
    }
}
