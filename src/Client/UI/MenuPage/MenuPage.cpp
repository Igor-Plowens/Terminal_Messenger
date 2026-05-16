#include "MenuPage.hpp"

MenuPage::MenuPage(Queue &queue): eventQueue(queue) {
    leaveButton = ftxui::Button("Leave", [this]() {
        InformationUnit act;
        act.opcode = CLIENT_SHUTDOWN;
        eventQueue.pushBack(act);
    });


    dmDestinationInput = ftxui::Input(&dmDestination, "Recipient name");
    enterDmButton = ftxui::Button("Enter dms", [this]() {
        InformationUnit act;
        act.opcode = GO_TO_DM_PAGE;

        //dmDestination = "";
        eventQueue.pushBack(act);
    });


    container = ftxui::Container::Vertical({dmDestinationInput, enterDmButton, leaveButton});
}


ftxui::Component MenuPage::getContainer() {
    return container;
}

std::string MenuPage::getChosenRecipient() {
    return dmDestination;
}
