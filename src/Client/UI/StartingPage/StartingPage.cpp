#include "StartingPage.hpp"

#include "Networker/Networker.hpp"
#include "UiState/UiState.hpp"


StartingPage::StartingPage(Queue &queue): eventQueue(queue) {
    goToLoginPageButton = ftxui::Button("Go to login", [this]() {

        Task task = [](UiState& uiState, Networker& networker) {
            uiState.selector = PageType::LOGIN_PAGE;
        };

        eventQueue.pushBack(std::move(task));
    });

    goToRegisterPageButton = ftxui::Button("Go to register", [this]() {
        Task task = [](UiState& uiState, Networker& networker) {
            uiState.selector = PageType::REGISTER_PAGE;
        };

        eventQueue.pushBack(std::move(task));
    });

    leaveButton = ftxui::Button("Leave", [this]() {
        Task task = [](UiState& uiState, Networker& networker) {
            networker.setShutdown();
            uiState.screen.Exit();
       };
        eventQueue.pushBack(std::move(task));
    });

    container = ftxui::Container::Horizontal({goToRegisterPageButton, goToLoginPageButton, leaveButton});
}

ftxui::Component StartingPage::getContainer() {
    return container;
}



