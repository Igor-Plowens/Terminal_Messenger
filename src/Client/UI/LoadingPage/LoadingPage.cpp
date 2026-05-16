#include "LoadingPage.hpp"


LoadingPage::LoadingPage(Queue &queue): eventQueue(queue) {
    renderer = ftxui::Renderer([this]() {
       return ftxui::paragraphAlignCenter("Loading...");
    });
}

ftxui::Component LoadingPage::getRenderer() {
    return renderer;
}

void LoadingPage::setMethodOfEntry(PageType method) {
    methodOfEntry = method;
}

PageType LoadingPage::getMethodOfEntry() {
    return methodOfEntry;
}

