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

    Queue(std::mutex &mut, std::condition_variable &cv): mut(mut), cv(cv) {}


    void pushBack(const InformationUnit &item) {
        {
            std::lock_guard lock(mut);
            queue.push_back(item);
        }
        cv.notify_all();
    }


    InformationUnit getFront() {
        std::lock_guard lock(mut);
        return queue.front();
    }

    bool isEmpty() {
        return queue.empty();
    }

    std::deque<InformationUnit> &getDeq() {
        return queue;
    }


private:
    std::deque<InformationUnit> queue;
    std::mutex &mut;
    std::condition_variable &cv;
};
