#include "Overseer.hpp"

#include "Parsing/parsing.hpp"


Overseer::Overseer(int sockFd, ftxui::ScreenInteractive &screen): eventQueue(screen), connection(sockFd), pageManager(eventQueue), screen(screen){}


void Overseer::networkReader() {

    while (true) {
        auto res = connection.performRead();
        myFile << "READ FINISHED\n";
        myFile.flush();
        if (res == Connection::FAILED) {
            myFile << "CONNECTION ERROR\n";
            myFile.flush();
            InformationUnit info;
            info.opcode = CLIENT_SHUTDOWN;
            eventQueue.pushBack(info);
            return;
        }
        myFile << "PARSING INFORMATION\n";
        myFile.flush();
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
            writeQueueCV.wait(lock, [this]() { return !writeQueue.empty(); });
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
    myFile << "HANDLE QUEUE WOKEN UP\n";
    myFile.flush();
    for (const auto &unit : units) {
        switch (unit.opcode) {
            case LOGIN:
            case REGISTER:
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
    myFile << " REACT TO INFO CALLED\n";
    myFile.flush();
    switch (unit.opcode) {
        case LOGIN_SUCCESS:
        case REGISTER_SUCCESS: {
            pageManager.setSelector(PageManager::MENU);
            myFile << "SET TO MENU\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_STARTING_PAGE: {
            pageManager.setSelector(PageManager::STARTING_PAGE);
            myFile << "SET TO STARTING PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_LOGIN_PAGE: {
            pageManager.setSelector(PageManager::LOGIN_PAGE);
            myFile << "SET TO LOGIN PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        case GO_TO_REGISTER_PAGE: {
            pageManager.setSelector(PageManager::REGISTER_PAGE);
            myFile << "SET TO REGISTER PAGE\n" << std::endl;
            myFile.flush();
            break;
        }
        default: {
            myFile << "BULLSHIT\n" << std::endl;
            myFile.flush();
        }
    }
    myFile.close();
    //screen.PostEvent(ftxui::Event::Custom);
}


ftxui::Component Overseer::getTab() {
    return pageManager.getTab();
}


