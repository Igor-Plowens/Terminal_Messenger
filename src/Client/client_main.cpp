#include <iostream>
#include <string>

#include "Overseer/Overseer.hpp"



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

    Overseer ovs(sock, screen);

    std::thread t1(&Overseer::networkReader, &ovs);
    std::thread t2(&Overseer::networkWriter, &ovs);
    auto catcher = ftxui::CatchEvent(ovs.getTab(), [&ovs](ftxui::Event ev) {
        if (ev == ftxui::Event::Custom) {
            ovs.handleQueue();
            return true;
        }
        return false;
    });

    screen.Loop(catcher);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::printf("Screen loop ended\n");
    t2.join();
    std::printf("network writer joined\n");
    t1.join();
    std::printf("network reader joined\n");

    return 0;
}

