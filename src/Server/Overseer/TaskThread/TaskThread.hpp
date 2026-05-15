#pragma once
#include <condition_variable>

#include "InformationUnit/InformationUnit.hpp"
#include "Database/Database.hpp"
#include "includes.hpp"
#include "TaskIncoming.hpp"


class TaskManager {
public:

    void assignTasks(std::vector<TaskIncoming> &&tasks);

    void setShutdown();


    std::optional<std::vector<TaskOutgoing>> processTasks();

private:

    std::optional<TaskOutgoing> processLogin(const TaskIncoming &task);
    std::optional<TaskOutgoing> processRegister(const TaskIncoming &task);
    std::optional<TaskOutgoing> processSendMessageByName(const TaskIncoming &task);
    //std::optional<TaskOutgoing> processGetFurtherMessages(const TaskIncoming &task); todo:
    //std::optional<TaskOutgoing> processGetLatestMessages(const TaskIncoming &task);



    Database db;

    std::vector<TaskIncoming> incomingTasks;
    std::mutex incomingTasksMut;
    std::condition_variable incomingTasksCv;


    bool shutdown = false;


};


