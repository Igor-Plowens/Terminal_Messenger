#include "PageManager.hpp"


PageManager::PageManager(Queue &queue): startingPage(queue), registerPage(queue), loginPage(queue), loadingPage(queue), menuPage(queue), dmPage(queue) {
    container = ftxui::Container::Tab({startingPage.getContainer(), registerPage.getRenderer(), loginPage.getRenderer(), loadingPage.getRenderer(), menuPage.getContainer(), dmPage.getRenderer()}, &selector);
}


ftxui::Component PageManager::getTab() {
    return container;
}

void PageManager::setSelector(PageType val) {
    if (val == LOADING_PAGE) {
        loadingPage.setMethodOfEntry(static_cast<PageType>(selector));
    }

    if (val == DM_PAGE) {
        dmPage.setRecipient(menuPage.getChosenRecipient());
    }

    selector = val;
}

PageType PageManager::getPageType() {
    auto res = static_cast<PageType>(selector);
    return res;
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

void PageManager::forwardDmMessage(DmMessage &&dmMessage) {
    dmPage.addMessage(std::move(dmMessage));
}

void PageManager::incrementDmCache() {
    dmPage.incrementOffset();
}
void PageManager::decrementDmCache() {
    dmPage.decrementOffset();
}


std::string PageManager::getDmRecipient() {
    return dmPage.getRecipient();
}
