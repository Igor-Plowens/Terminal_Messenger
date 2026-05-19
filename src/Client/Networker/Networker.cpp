#include "Networker.hpp"
#include "Parsing/parsing.hpp"
#include "UiState/UiState.hpp"


Networker::Networker(int sockfd, Queue &eventQueue): connection(sockfd), eventQueue(eventQueue) {}


void Networker::setShutdown() {
    {
        std::lock_guard lck(writeQueueMut);
        shutdownFlag = true;
        shutdown(connection.getSocket(), SHUT_RDWR);
        close(connection.getSocket());
    }
    writeQueueCV.notify_all();
}

void Networker::queueWrite(InformationUnit unit) {
    {
        std::lock_guard lck(writeQueueMut);
        writeQueue.push_back(std::move(unit));
    }
    writeQueueCV.notify_all();
}


void Networker::networkReadingThread() {
    while (true) {
        auto res = connection.performRead();
        if (res == Connection::FAILED) {
            if (shutdownFlag) return; //task or writer called setShutdown
            setShutdown();
            Task task = [](UiState& state, Networker&networker) {

                state.screen.Exit();
            };
            eventQueue.pushBack(std::move(task));
            return;
        }
        InformationUnit unit = Parsing::parse_buffer(connection.relinquish_buff());
        Task task = convertInformationToTask(unit);
        eventQueue.pushBack(std::move(task));
    }
}

void Networker::networkWritingThread() {
    while (true) {
        InformationUnit unit;
        {
            std::unique_lock<std::mutex> lock(writeQueueMut);
            writeQueueCV.wait(lock, [this]() { return !writeQueue.empty() || shutdownFlag; });
            if (shutdownFlag) return; //task or reader called shutdown
            unit = writeQueue.front();
            writeQueue.pop_front();
        }
        bool test = connection.performWrite(Parsing::imprint_buffer(unit));
        if (!test) {
            setShutdown();
            Task task = [](UiState& state, Networker&networker) {
                state.screen.Exit();
            };
            eventQueue.pushBack(std::move(task));
            return;
        }
    }
}


Task Networker::convertInformationToTask(InformationUnit unit) {
    Task task;
    switch (unit.opcode) {
        case LOGIN_FAIL: {
            task = [](UiState& state, Networker&networker) {
                state.loginPage.setFeedback("Login failure!");
                state.selector = PageType::LOGIN_PAGE;
            };
            return task;
        }
        case REGISTER_FAILURE: {
            task = [](UiState& state, Networker&networker) {
                state.registerPage.setFeedback("Register failure!");
                state.selector = PageType::REGISTER_PAGE;
            };
            return task;
        }

        case REGISTER_SUCCESS:
        case LOGIN_SUCCESS: {
            ID_t id = std::get<ID_t>(unit.data[0]);
            std::string nick = std::get<std::string>(unit.data[1]);
            task = [id, nick](UiState& state, Networker&networker) {
                //todo: clear inputs in login/register if logout is to be implemented
                //todo: it will require splitting those two cases into one
                state.myNickname = nick;
                state.myId = id;
                state.selector = PageType::MENU_PAGE;
            };
            return task;
        }


        case RELAY_MESSAGE_BY_NAME: {
            std::string authorName = std::get<std::string>(unit.data[0]);
            ID_t messageId = std::get<ID_t>(unit.data[1]);
            std::string content = std::get<std::string>(unit.data[2]);
            task = [authorName, messageId, content](UiState& state, Networker&networker) {
                if (state.selector != PageType::DM_PAGE) {
                    return;
                }
                if (!(authorName == state.myNickname || authorName == state.dmPage.getRecipient())) {
                    return;
                }

                DmMessage mess;
                if (authorName == state.myNickname) {
                    mess.isMine = true;
                }
                else {
                    mess.isMine = false;
                }
                mess.messageId = messageId;
                mess.content = content;
                state.dmPage.addMessageBeginning(std::move(mess));
            };
            return task;
        }

        case RELAY_LATEST_MESSAGES_BY_NAME: {
            std::vector<DmMessage> messages;
            for (int i = 2, j = 0; j < std::get<std::uint16_t>(unit.data[1]); i+=3, j++) {
                messages.emplace_back(std::get<Byte>(unit.data[i+1]),
                    std::get<std::string>(unit.data[i+2]),
                    std::get<ID_t>(unit.data[i]));
            }
            task = [messages](UiState& state, Networker&networker) mutable {
                for (auto it = messages.begin(); it != messages.end(); ++it ) {
                    state.dmPage.addMessageEnd(std::move(*it));
                }
                state.selector = PageType::DM_PAGE;
            };
            return task;
        }
        case RELAY_FURTHER_MESSAGES_BY_NAME: {
            std::vector<DmMessage> messages;
            for (int i = 2, j = 0; j < std::get<std::uint16_t>(unit.data[1]); i+=3, j++) {
                messages.emplace_back(std::get<Byte>(unit.data[i+1]),
                    std::get<std::string>(unit.data[i+2]),
                    std::get<ID_t>(unit.data[i]));
            }
            task = [messages](UiState& state, Networker&networker) mutable {
                for (auto it = messages.begin(); it != messages.end(); ++it ) {
                    state.dmPage.addMessageEnd(std::move(*it));
                }
                if (messages.size() < 10) {
                    state.dmPage.setDmMessagesCacheFull();
                }
            };
            return task;

        }
        default: {
            throw std::runtime_error("Invalid opcode");
        }
            //todo: relaying further/latest messages
    }
}
