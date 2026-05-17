#include "Overseer.hpp"

#include "Parsing/parsing.hpp"


Overseer::Overseer(int sockFd, ftxui::ScreenInteractive &screen): eventQueue(screen), connection(sockFd), pageManager(eventQueue), screen(screen){}


void Overseer::setShutdown() {
    shutdownFlag = true;
    shutdown(connection.getSocket(), SHUT_RDWR);
    close(connection.getSocket());
    writeQueueCV.notify_all();
}

void Overseer::networkReader() {

    while (true) {
        auto res = connection.performRead();
        myFile << "READ FINISHED\n";
        myFile.flush();
        if (res == Connection::FAILED) {
            myFile << "CONNECTION ERROR\n";
            if (shutdownFlag) return;
            myFile.flush();
            InformationUnit info;
            info.opcode = CLIENT_SHUTDOWN;
            eventQueue.pushBack(info);
            return;
        }
        InformationUnit unit = Parsing::parse_buffer(connection.relinquish_buff());
        myFile << "PARSED OPCODE: " << unit.opcode << "\n";
        myFile.flush();
        eventQueue.pushBack(unit);
    }
}

void Overseer::networkWriter() {

    while (true) {
        InformationUnit info;
        {
            std::unique_lock<std::mutex> lock(writeQueueMut);
            writeQueueCV.wait(lock, [this]() { return !writeQueue.empty() || shutdownFlag; });
            if (shutdownFlag) return;
            info = writeQueue.front();
            writeQueue.pop_front();
        }
        bool test = connection.performWrite(Parsing::imprint_buffer(info));
        if (!test) {
            InformationUnit res;
            res.opcode = CLIENT_SHUTDOWN;
            eventQueue.pushBack(res);
            return;
        }
    }
}


void Overseer::handleQueue() {


    std::deque<InformationUnit> units;
    {

        eventQueue.getQueue(units);
    }
    for (const auto &unit : units) {
        switch (unit.opcode) {
            case CLIENT_SHUTDOWN: {
                screen.Exit();
                setShutdown();
                return;
            }
            case LOGIN:
            case REGISTER:
                sendInfo(unit);
                reactToInfo(unit);
                break;

            case SEND_MESSAGE_BY_NAME:
                sendInfo(unit);
                break;
            default:
                reactToInfo(unit);
        }
    }
    units.clear();


}




void Overseer::sendInfo(const InformationUnit &unit) {
    {
        std::lock_guard lock(writeQueueMut);
        writeQueue.push_back(unit);
    }
    writeQueueCV.notify_all();
}

void Overseer::reactToInfo(const InformationUnit &unit) {
    myFile << "REACT TO INFO CALLED\n";
    myFile.flush();
    switch (unit.opcode) {
        case LOGIN:
        case REGISTER:
            pageManager.setSelector(LOADING_PAGE);
            break;



        case REGISTER_FAILURE:
        case LOGIN_FAIL:
            pageManager.forwardFeedbackString("Login/Register failed");
            pageManager.setSelector(pageManager.getLoadingsMethodOfEntry());
            break;



        case LOGIN_SUCCESS:
        case REGISTER_SUCCESS: {
            //pageManager.forwardFeedbackString("Input data"); todo: add if logout and login?
            pageManager.nickname = std::get<std::string>(unit.data[1]);
            pageManager.setSelector(MENU);
            myFile << "SET TO MENU\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_STARTING_PAGE: {
            pageManager.setSelector(STARTING_PAGE);
            myFile << "SET TO STARTING PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_LOGIN_PAGE: {
            pageManager.setSelector(LOGIN_PAGE);
            myFile << "SET TO LOGIN PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_REGISTER_PAGE: {
            pageManager.setSelector(REGISTER_PAGE);
            myFile << "SET TO REGISTER PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_MENU_PAGE: {
            pageManager.setSelector(MENU);
            break;
        }

        case GO_TO_DM_PAGE: {
            myFile << "GO TO DM PAGE\n";
            myFile.flush();
            pageManager.setSelector(DM_PAGE);
            break;
        }
        case RELAY_MESSAGE_BY_NAME: {
            myFile << "RELAY MESSAGE BY NAME BEING PARSED\n";
            myFile.flush();
            std::string author = std::get<std::string>(unit.data[0]);
            DmMessage message;
            if (author == pageManager.nickname) {
                message.isMine = true;
                myFile << "MESSAGE IS MINE\n";
                myFile.flush();
            }
            else if (author == pageManager.getDmRecipient()) {
                myFile << "MESSAGE IS NOT MINE BUT VALID\n";
                myFile.flush();
                message.isMine = false;
            }
            else {
                myFile << "MESSAGE IS STALE\n";
                myFile.flush();
                break; //stale message
            }

            message.messageId = std::get<ID_t>(unit.data[1]);
            message.content = std::get<std::string>(unit.data[2]);
            pageManager.forwardDmMessage(std::move(message));
            break;
        }

        default: {
            throw std::runtime_error("Unknown opcode [reactToInfo]");
        }
    }
    myFile.close();
}


ftxui::Component Overseer::getTab() {
    return pageManager.getTab();
}


