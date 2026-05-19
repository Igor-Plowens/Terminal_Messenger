#include "UiState/UiState.hpp"
#include "InformationUnit/InformationUnit.hpp"
#include "Networker/Networker.hpp"

MenuPage::MenuPage(Queue &queue): eventQueue(queue) {
    leaveButton = ftxui::Button("Leave", [this]() {
        Task task = [](UiState& uiState, Networker& networker) {
            networker.setShutdown();
            uiState.screen.Exit();
       };
        eventQueue.pushBack(std::move(task));
    });


    dmDestinationInput = ftxui::Input(&dmDestination, "Recipient name");
    enterDmButton = ftxui::Button("Enter dms", [this]() {
        Task task = [this](UiState& uiState, Networker& networker) {
            uiState.dmPage.setRecipient(dmDestination);
            uiState.selector = PageType::LOADING_PAGE;

            InformationUnit unit;
            unit.opcode = GET_LATEST_MESSAGES_BY_NAME;
            unit.append_val(dmDestination);
            networker.queueWrite(std::move(unit));
            dmDestination.clear();
       };
        eventQueue.pushBack(std::move(task));
    });


    container = ftxui::Container::Vertical({dmDestinationInput, enterDmButton, leaveButton});
}


ftxui::Component MenuPage::getContainer() {
    return container;
}

std::string MenuPage::getChosenRecipient() {
    return dmDestination;
}
