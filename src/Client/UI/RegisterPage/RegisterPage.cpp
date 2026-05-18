#include "UiState/UiState.hpp"
#include "InformationUnit/InformationUnit.hpp"
#include "Networker/Networker.hpp"

RegisterPage::RegisterPage(Queue &queue): eventQueue(queue) {
    inputUsername = ftxui::Input(&username, "Input your username");
    inputPassword = ftxui::Input(&password, "Input your password");

    backToStartingButton = ftxui::Button("Back to Starting", [this]() {
        Task task = [this](UiState& state, Networker& networker) {
            state.selector = PageType::STARTING_PAGE;
            username.clear();
            password.clear();
        };
        eventQueue.pushBack(task);
    });


    sendButton = ftxui::Button("Send", [this]() {
        Task task = [this](UiState& state, Networker& networker) {
            state.selector = PageType::LOADING_PAGE;
            InformationUnit act;
            act.opcode = REGISTER;
            act.append_val(username);
            act.append_val(password);
            username.clear();
            password.clear();
            networker.queueWrite(std::move(act));
        };
        eventQueue.pushBack(task);
    });

    container = ftxui::Container::Vertical({inputUsername, inputPassword, sendButton, backToStartingButton});

    renderer = ftxui::Renderer(container, [this]() {
        return ftxui::vbox({
            ftxui::paragraphAlignCenter(feedback),
            container->Render()
        });
    });
}


ftxui::Component RegisterPage::getRenderer() {
    return renderer;

}

void RegisterPage::setFeedback(const std::string &text) {
    feedback = text;
}

// ftxui::Component RegisterPage::getContainer() {
//     return container;
// }

