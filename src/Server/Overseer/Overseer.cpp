

#include "Overseer.hpp"


Overseer::Overseer(int wakeupFd): networkManager(wakeupFd) {}


void Overseer::startShutdown() {
    taskManager.setShutdown();
    networkManager.shutdown();
}


void Overseer::networkLoop() {
    while (true) {
        std::optional<std::vector<TaskIncoming>> tasks = networkManager.loopOver();
        if (!tasks.has_value()) {
            return;
        }
        taskManager.assignTasks(std::move(*tasks));
    }
}

void Overseer::taskLoop() {
    while (true) {
        std::optional<std::vector<TaskOutgoing>> tasks = taskManager.processTasks();
        if (!tasks.has_value()) {
            return;
        }
        networkManager.assignTasks(std::move(*tasks));
    }
}

