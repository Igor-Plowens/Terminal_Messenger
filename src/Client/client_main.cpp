#include <iostream>
#include <string>

#include "Networker/Networker.hpp"
#include "Task/Task.hpp"
#include "UiState/UiState.hpp"



constexpr int portNumber = 8080;
constexpr const char * ipAddress = "127.0.0.1";
int main(int argc, char** argv) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);
    serv_addr.sin_addr.s_addr = inet_addr(ipAddress);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        return 1;
    }




    auto screen = ftxui::ScreenInteractive::Fullscreen();

    Queue eventQueue(screen);

    Networker networker(sock, eventQueue);

    UiState uiState(screen, eventQueue);


    std::thread t1(&Networker::networkReadingThread, &networker);
    std::thread t2(&Networker::networkWritingThread, &networker);
    auto catcher = ftxui::CatchEvent(uiState.getFinalContainer(),
        [&eventQueue, &networker, &uiState](ftxui::Event ev) {
        if (ev == ftxui::Event::Custom) {
            std::deque<Task> tasksCopy;
            eventQueue.getQueue(tasksCopy);
            for (auto &task: tasksCopy) {
                task(uiState, networker);
            }
            return true;
        }

        if (ev.is_mouse()) {
            if (uiState.selector != PageType::DM_PAGE) return false;

            if (ev.mouse().button == ftxui::Mouse::WheelUp) {
                uiState.dmPage.decrementOffset();
                return true;
            }
            if (ev.mouse().button == ftxui::Mouse::WheelDown) {
                uiState.dmPage.incrementOffset();
                return true;
            }
        }

        return false;
    });

    screen.Loop(catcher);
    std::printf("Screen loop ended\n");
    t2.join();
    std::printf("network writer joined\n");
    t1.join();
    std::printf("network reader joined\n");

    return 0;
}

