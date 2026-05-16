#include "MenuPage.hpp"

MenuPage::MenuPage(Queue &queue): eventQueue(queue) {
    leaveButton = ftxui::Button("Leave", [this]() {
        InformationUnit act;
        act.opcode = CLIENT_SHUTDOWN;
        eventQueue.pushBack(act);
    });

    container = ftxui::Container::Vertical({leaveButton});
}


ftxui::Component MenuPage::getContainer() {
    return container;
}
