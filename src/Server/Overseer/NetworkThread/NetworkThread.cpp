//
// Created by jawn on 5/7/26.
//

#include "NetworkThread.hpp"
#include "Parsing/parsing.hpp"



NetworkThread::NetworkThread(int wakeupFd): wakeupFd(wakeupFd) {
    listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listenFd == -1) {
        throw std::runtime_error("Failed to create listening socket");
    }
    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    if (bind(listenFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::runtime_error("Failed to bind listening socket");
    }
    if (listen(listenFd, 1024) == -1) {
        throw std::runtime_error("Failed to listen on listening socket");
    }

    epoll_manager.addEvent(listenFd);
    epoll_manager.addEvent(wakeupFd);

}


void NetworkThread::assignTasks(std::vector<TaskOutgoing> &&tasks) {
    {
        std::lock_guard lock(tasksMut);
        this->tasks = std::move(tasks);
    }
    wakeup();
}

void NetworkThread::shutdown() {
    {
        std::lock_guard lck(shutdownMut);
        shutdownFlag = true;
    }
    wakeup();
}


void NetworkThread::wakeup() {
    std::uint64_t val = 1;
    write(wakeupFd, static_cast<const void *>(&val), sizeof(val));
}

#define DEBUG

std::optional<std::vector<TaskIncoming>>NetworkThread::loopOver() {

    std::vector<TaskIncoming> res;

    ReadyList list = epoll_manager.wait();

    {
        std::lock_guard lck(shutdownMut);
        if (shutdownFlag) {
            return std::nullopt;
        }
    }

    std::printf("New messages arrived [loopOver]\n");

    std::vector<TaskOutgoing> tasksLocal;
    {
        std::lock_guard lck(tasksMut);
        if (!tasks.empty()) {
            tasks.swap(tasksLocal);
        }
    }
    if (!tasksLocal.empty()) {
        std::printf("Calling loopOverTasks... [loopOver]\n");
        loopOverTasks(tasksLocal);
    }

    for (Size i = 0; i < list.count; ++i) {
        if (list.buffer[i].data.fd == listenFd) {
            std::printf("Adding a client [loopOver]\n");
            addClient();
            continue;
        }
        if (list.buffer[i].data.fd == wakeupFd) {
            std::uint64_t val;
            read(wakeupFd, static_cast<void *>(&val), sizeof(val));
            continue;
        }

        ClientData data;
        data.connectionId = list.buffer[i].data.fd;
        std::shared_ptr<Client> foundClient = clients.at(data);

        std::optional<InformationUnit> unit = handleClient(foundClient, list.buffer[i]);

        if (unit) {
            res.emplace_back(foundClient->getData(), std::move(*unit));
        }
    }
    return res;
}


std::optional<InformationUnit> NetworkThread::handleClient(std::shared_ptr<Client> client, const epoll_event &ev) {
    std::printf("Client of connection id: %d is being handled [handleClient]\n", ev.data.fd);
    std::optional<InformationUnit> res = std::nullopt;
    if (ev.events & EPOLLIN) {
        std::printf("There is data to read [handleClient]\n");
        auto check = client->conn.perform_read();
        if (check == Connection::DISCONNECT) {
            std::printf("Client disconnected [handleClient]\n");
            clients.erase(client->getData());
            epoll_manager.removeEvent(ev.data.fd);
            return std::nullopt;
        }
        if (check == Connection::COMPLETE) {
            std::printf("Read was complete [handleClient]\n");
            res = Parsing::parse_buffer(client->conn.relinquish_buffer());
        }
        else {
            std::printf("Read was incomplete [handleClient]\n");
        }
    }
    if (ev.events & EPOLLOUT) {
        auto check = client->conn.perform_write();
        if (check == Connection::DISCONNECT) {
            std::printf("Client disconnected [handleClient]\n");
            clients.erase(client->getData());
            epoll_manager.removeEvent(ev.data.fd);
            return std::nullopt;
        }
        if (check == Connection::COMPLETE) {
            std::printf("Write was complete [handleClient]\n");
            epoll_manager.removeEpollout(ev.data.fd);
        }
        else {
            std::printf("Write was incomplete [handleClient]\n");
        }
    }

    return res;
}

void NetworkThread::loopOverTasks(const std::vector<TaskOutgoing> &tasks) {
    for (const auto &task : tasks) {
        std::printf("Task of opcode: %d is being distributed [loopOverTasks]\n", task.information.opcode);
        for (auto &destinationAddress: task.recipients) {
            auto found = clients.find(destinationAddress);
            if (found != clients.end()) {
                std::printf("Task is being distributed to sockid: %d\n", (*found).second->conn.get_sock());
                (*found).second->conn.assign_write(Parsing::imprint_buffer(task.information));
                epoll_manager.giveEpollout((*found).second->conn.get_sock());
            }
            if (task.information.opcode == LOGIN_SUCCESS || task.information.opcode == REGISTER_SUCCESS) {
                (*found).second->setUserInfo(std::get<ID_t>(task.information.data[0]), std::get<std::string>(task.information.data[1]));
            }
        }
    }
}

void NetworkThread::addClient() {
    int sock = accept4(listenFd, nullptr, nullptr, SOCK_NONBLOCK);
    if (sock == -1) {
        std::printf("Failed to accept client connection\n");
        return;
    }
    std::printf("Client of sock %d was added [addClient]\n", sock);
    ClientData data;
    data.connectionId = sock;
    clients.emplace(data, std::make_shared<Client>(sock));
    epoll_manager.addEvent(sock);
}
