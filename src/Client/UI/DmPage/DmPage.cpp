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
            clearCache();
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

    // renderer = ftxui::Renderer(container, [this]() {
    //     ftxui::Elements elems;
    //     {
    //         for (int i = 0 + offset, j = 0; i < dmMessagesCache.size() && j < limit; i++, j++) {
    //             if (dmMessagesCache[i].isMine) {
    //                 elems.push_back(ftxui::paragraphAlignLeft(dmMessagesCache[i].content) | ftxui::border);
    //             }
    //             else {
    //                 elems.push_back(ftxui::paragraphAlignRight(dmMessagesCache[i].content) | ftxui::border);
    //             }
    //         }
    //     }
    //     return ftxui::vbox({
    //         ftxui::vbox(elems) | ftxui::border | ftxui::flex,
    //         container->Render()
    //     }
    //     );
    // });

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
            ftxui::text(std::to_string(offset)),
            ftxui::text(std::to_string(dmMessagesCache.size())),
            ftxui::vbox(elems) | ftxui::border | ftxui::flex,
            container->Render()
        }
        );
    });
}

/*
void DmPage::decrementOffset() {
    offset = std::max(0, offset - 1);
}

void DmPage::incrementOffset() {
    offset = std::min(static_cast<int>(dmMessagesCache.size() - 1), offset + 1);
}
*/

void DmPage::decrementOffset() {
    offset = std::max(0, offset - 1);
}


std::optional<Task> DmPage::incrementOffset() {
    int potentialNewOffset = std::min(static_cast<int>(dmMessagesCache.size() - 1), offset + 1);
    if (potentialNewOffset + limit > dmMessagesCache.size()) {
        if (dmMessagesCacheFull) return std::nullopt;
        Task task = [this](UiState &uiState, Networker &networker) {
            InformationUnit unit;
            unit.opcode = GET_FURTHER_MESSAGES_BY_NAME;
            unit.append_val(recipient);
            unit.append_val(dmMessagesCache.back().messageId);
            networker.queueWrite(unit);
        };
        return task;
    }
    offset = potentialNewOffset;
    return std::nullopt;
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

void DmPage::addMessageEnd(DmMessage &&message) {
    dmMessagesCache.push_back(std::move(message));
}

void DmPage::addMessageBeginning(DmMessage &&message) {
    dmMessagesCache.push_front(std::move(message));
}


void DmPage::clearCache() {
    recipient.clear();
    dmMessagesCache.clear();
    offset = 0;
    setDmMessagesCacheNotFull();
}

void DmPage::setDmMessagesCacheFull() {
    dmMessagesCacheFull = true;
}

void DmPage::setDmMessagesCacheNotFull() {
    dmMessagesCacheFull = false;
}
