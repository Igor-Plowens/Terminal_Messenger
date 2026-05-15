#include <iostream>

#include "Overseer/Overseer.hpp"


int main(int argc, char* argv[]) {
    int wakeup = eventfd(0, EFD_NONBLOCK);
    if (wakeup == 1) {
        std::printf("Eventfd failed\n");
        return 1;
    }

    Overseer ovs(wakeup);

    std::thread t1(&Overseer::networkLoop, &ovs);
    std::thread t2(&Overseer::taskLoop, &ovs);

    std::printf("Type enter to shutdown\n");
    std::string temp;
    std::getline(std::cin, temp);

    ovs.startShutdown();
    std::printf("Shutdown started\n");


    t2.join();
    std::printf("Task loop joined\n");

    t1.join();
    std::printf("Network loop joined\n");
    return 0;
}