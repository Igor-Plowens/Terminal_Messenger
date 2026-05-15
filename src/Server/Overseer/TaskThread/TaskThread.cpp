#include "TaskThread.hpp"


void TaskManager::assignTasks(std::vector<TaskIncoming> &&tasks) {
    {
        std::lock_guard lock(incomingTasksMut);
        incomingTasks = std::move(tasks);
    }
    incomingTasksCv.notify_one();
}


void TaskManager::setShutdown() {
    {
        std::lock_guard lock(incomingTasksMut);
        shutdown = true;
    }
    incomingTasksCv.notify_one();
}


std::optional<std::vector<TaskOutgoing>> TaskManager::processTasks() {

    std::vector<TaskIncoming> tasks;

    {
        std::unique_lock lock(incomingTasksMut);
        incomingTasksCv.wait(lock, [this]() { return !incomingTasks.empty() || shutdown; });
        std::printf("processTasks was woken up [processTasks]\n");
        if (shutdown) {
            std::printf("Leaving processTasks due to shutdown [processTasks]\n");
            return std::nullopt;
        }
        tasks.swap(incomingTasks);
    }

    std::vector<TaskOutgoing> tasksOutgoing;

    for (const TaskIncoming &task : tasks) {
        std::optional<TaskOutgoing> res;
        if (task.information.opcode == LOGIN) {
            std::printf("handling login [processTasks]\n");
            res = processLogin(task);
        }
        else if (task.information.opcode == REGISTER) {
            std::printf("handling register [processTasks]\n");
            res = processRegister(task);
        }
        else if (task.information.opcode == SEND_MESSAGE_BY_NAME) {
            std::printf("handling send message by name [processTasks]\n");
            res = processSendMessageByName(task);
        }
        else {
            throw std::runtime_error("unknown opcode");
        }

        if (res) tasksOutgoing.push_back(std::move(*res));
    }
    return tasksOutgoing;
}


std::optional<TaskOutgoing> TaskManager::processLogin(const TaskIncoming &task) {
    std::printf("In processLogin [processLogin]\n");
    std::printf("Author of this task: %d\n", task.author.connectionId);
    std::string nickname = std::get<std::string>(task.information.data[0]);
    std::string password = std::get<std::string>(task.information.data[1]);
    std::printf("His nick and password: (%s, %s) [processLogin]\n", nickname.c_str(), password.c_str());
    bool check = db.check_if_user_credentials_match(nickname, password);
    TaskOutgoing res;
    ClientData data;
    data.connectionId = task.author.connectionId;
    res.recipients.push_back(data);
    if (check) {
        std::printf("Login success [processLogin]\n");
        res.information.opcode = LOGIN_SUCCESS;
    }
    else {
        std::printf("Login failure [processLogin]\n");
        res.information.opcode = LOGIN_FAIL;
    }
    res.information.append_val(static_cast<ID_t>(db.find_user_id(nickname)));
    res.information.append_val(static_cast<std::string>(nickname));
    return res;
}

std::optional<TaskOutgoing> TaskManager::processRegister(const TaskIncoming &task) {
    std::printf("In processRegsiter [processRegister]\n");
    std::printf("Author of this task: %d\n", task.author.connectionId);

    std::string nickname = std::get<std::string>(task.information.data[0]);
    std::string password = std::get<std::string>(task.information.data[1]);
    std::printf("His nick and password: (%s, %s) [processRegister]\n", nickname.c_str(), password.c_str());
    bool check = db.check_if_user_exists(nickname);
    TaskOutgoing res;
    ClientData data;
    data.connectionId = task.author.connectionId;
    res.recipients.push_back(data);
    if (check) {
        std::printf("Register failure [processRegister]\n");
        res.information.opcode = REGISTER_FAILURE;
    }
    else {
        std::printf("Register success [processRegister]\n");
        res.information.opcode = REGISTER_SUCCESS;
        db.add_user(nickname, password);
    }
    res.information.append_val(static_cast<ID_t>(db.find_user_id(nickname)));
    res.information.append_val(static_cast<std::string>(nickname));
    return res;
}

std::optional<TaskOutgoing> TaskManager::processSendMessageByName(const TaskIncoming &task) {
    std::string recipient = std::get<std::string>(task.information.data[0]);
    std::string message = std::get<std::string>(task.information.data[1]);

    int64_t recipientId = db.find_user_id(recipient);
    int64_t messageId = db.update_message(*task.author.userId, recipientId, message);

    TaskOutgoing res;
    ClientData data;
    data.connectionId = recipientId; //redundant
    data.username = recipient;

    res.recipients.push_back(data);
    res.information.opcode = RELAY_MESSAGE_BY_NAME;
    res.information.append_val(*task.author.username);
    res.information.append_val(static_cast<ID_t>(messageId));
    res.information.append_val(message);
    return res;
}








