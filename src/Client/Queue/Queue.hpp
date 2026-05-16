#pragma once
#include <deque>
#include <mutex>
#include "InformationUnit/InformationUnit.hpp"
#include "condition_variable"
#include "mutex"

// template<class T>
// class Queue {
// public:
//     void pushBack(T item) {
//         std::lock_guard lock(mutex);
//         queue.push_back(std::move(item));
//     }
//
//     T getFront() {
//         std::lock_guard lock(mutex);
//         return queue.front();
//     }
//
// private:
//     std::deque<T> queue;
//     std::mutex mutex;
// };

class Queue {
public:
    Queue(ftxui::ScreenInteractive &screen): screen(screen) {}


    void pushBack(const InformationUnit &item) {
        {
            std::lock_guard lock(mut);
            queue.push_back(item);
        }
        screen.PostEvent(ftxui::Event::Custom);
    }


    InformationUnit getFront() {
        std::lock_guard lock(mut);
        return queue.front();
    }

    bool isEmpty() {
        return queue.empty();
    }

    void getQueue(std::deque<InformationUnit> &inputQueue) {
        std::lock_guard lck(mut);
        inputQueue = std::move(queue);
        queue.clear();
    }


private:
    std::deque<InformationUnit> queue;
    std::mutex mut;

    ftxui::ScreenInteractive &screen;
};
