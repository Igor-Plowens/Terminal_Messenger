#include "InformationUnit/InformationUnit.hpp"
#include "Networker/Networker.hpp"
#include "UiState/UiState.hpp"
#include <algorithm>


constexpr int limit = 5;

DmPage::DmPage(Queue &queue): eventQueue(queue) {
    messageInput = ftxui::Input(&messageStr, "Message Input");
    sendButton = ftxui::Button("Send", [this]() {

        Task task = [this](UiState &uiState, Networker &networker) {
            InformationUnit unit;
            unit.opcode = SEND_MESSAGE_BY_NAME;
            unit.append_val(recipient);
            unit.append_val(messageStr);
            messageStr = "";
            networker.queueWrite(unit);
        };
        eventQueue.pushBack(task);
    });

    backToMenuButton = ftxui::Button("Go back to menu", [this]() {
        Task task = [this](UiState &uiState, Networker &networker) {
            offset = 0;
            uiState.selector = PageType::MENU_PAGE;
        };
        eventQueue.pushBack(task);
    });

    container = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({
            messageInput,
            sendButton
        }),
        backToMenuButton
    });

    renderer = ftxui::Renderer(container, [this]() {
        ftxui::Elements elems;
        {
            for (int i = 0 + offset, j = 0; i < dmMessagesCache.size() && j < limit; i++, j++) {
                if (dmMessagesCache[i].isMine) {
                    elems.push_back(ftxui::paragraphAlignLeft(dmMessagesCache[i].content) | ftxui::border);
                }
                else {
                    elems.push_back(ftxui::paragraphAlignRight(dmMessagesCache[i].content) | ftxui::border);
                }
            }
        }
        return ftxui::vbox({
            ftxui::vbox(elems) | ftxui::border | ftxui::flex,
            container->Render()
        }
        );
    });
}

void DmPage::decrementOffset() {
    offset = std::max(0, offset - 1);
}

void DmPage::incrementOffset() {
    offset = std::min(static_cast<int>(dmMessagesCache.size() - 1), offset + 1);
}


ftxui::Component DmPage::getRenderer() {
    return renderer;
}

void DmPage::setRecipient(const std::string &rec) {
    recipient = rec;
}

std::string DmPage::getRecipient() const {
    return recipient;
}

void DmPage::addMessage(DmMessage &&message) {
    dmMessagesCache.push_front(std::move(message));
}


void DmPage::clearCache() {
    dmMessagesCache.clear();
    offset = 0;
}