
#include "RegisterPage.hpp"
RegisterPage::RegisterPage(Queue &queue): eventQueue(queue) {
    inputUsername = ftxui::Input(&username, "Input your username");
    inputPassword = ftxui::Input(&password, "Input your password");

    backToStartingButton = ftxui::Button("Back to Starting", [this]() {
        InformationUnit act;
        act.opcode = GO_TO_STARTING_PAGE;
        eventQueue.pushBack(act);
    });


    sendButton = ftxui::Button("Send", [this]() {
        InformationUnit act;
        act.opcode = REGISTER;
        act.append_val(username);
        act.append_val(password);
        eventQueue.pushBack(act);
    });

    container = ftxui::Container::Vertical({inputUsername, inputPassword, sendButton, backToStartingButton});
}


ftxui::Component RegisterPage::getContainer() {
    return container;
}

