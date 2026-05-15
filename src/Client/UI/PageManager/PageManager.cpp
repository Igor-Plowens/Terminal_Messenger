#include "PageManager.hpp"


PageManager::PageManager(Queue &queue): startingPage(queue), registerPage(queue), loginPage(queue) {
    container = ftxui::Container::Tab({startingPage.getContainer(), registerPage.getContainer(), loginPage.getContainer(), menuPage.getContainer()}, &selector);
}


ftxui::Component PageManager::getTab() {
    return container;
}

void PageManager::setSelector(PageType val) {
    selector = val;
}


