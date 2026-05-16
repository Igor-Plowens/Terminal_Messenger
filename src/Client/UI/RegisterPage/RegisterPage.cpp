
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

