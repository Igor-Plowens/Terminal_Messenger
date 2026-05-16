#include "StartingPage.hpp"


StartingPage::StartingPage(Queue &queue): eventQueue(queue) {
    goToLoginPageButton = ftxui::Button("Go to login", [this]() {

        InformationUnit act;
        act.opcode = GO_TO_LOGIN_PAGE;
        eventQueue.pushBack(act);
    });

    goToRegisterPageButton = ftxui::Button("Go to register", [this]() {
        InformationUnit act;
        act.opcode = GO_TO_REGISTER_PAGE;
        eventQueue.pushBack(act);
    });

    leaveButton = ftxui::Button("Leave", [this]() {
        InformationUnit act;
        act.opcode = CLIENT_SHUTDOWN;
        eventQueue.pushBack(act);
    });

    container = ftxui::Container::Horizontal({goToRegisterPageButton, goToLoginPageButton, leaveButton});
}

ftxui::Component StartingPage::getContainer() {
    return container;
}



