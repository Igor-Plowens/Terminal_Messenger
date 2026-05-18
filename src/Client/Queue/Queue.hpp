#pragma once
#include <deque>
#include <mutex>
#include "Task/Task.hpp"
#include "condition_variable"
#include "mutex"
#include "includesFTXUI.hpp"


class Queue {
public:
    Queue(ftxui::ScreenInteractive &screen): screen(screen) {}


    void pushBack(Task item) {
        {
            std::lock_guard lock(mut);
            queue.push_back(std::move(item));
        }
        screen.PostEvent(ftxui::Event::Custom);
    }

    void getQueue(std::deque<Task> &inputQueue) {
        std::lock_guard lck(mut);
        inputQueue = std::move(queue);
        queue.clear();
    }


private:
    std::deque<Task> queue;
    std::mutex mut;
    ftxui::ScreenInteractive &screen;
};
