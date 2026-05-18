#include "TaskThread.hpp"
#include <iostream>

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
        else if (task.information.opcode == GET_LATEST_MESSAGES_BY_NAME) {
            std::printf("Getting latest messages by name [processTasks]\n");
            res = processGetLatestMessagesByName(task);
        }
        else {
            throw std::runtime_error("unknown opcode");
        }

        if (res) tasksOutgoing.push_back(std::move(*res));
    }
    return tasksOutgoing;
}


std::optional<TaskOutgoing> TaskManager::processLogin(const TaskIncoming &task) {
    std::this_thread::sleep_for(std::chrono::seconds(1)); //todo: remove

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
        res.information.append_val(static_cast<ID_t>(db.find_user_id(nickname)));
        res.information.append_val(static_cast<std::string>(nickname));
    }
    else {
        std::printf("Login failure [processLogin]\n");
        res.information.opcode = LOGIN_FAIL;
    }
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
        res.information.append_val(static_cast<ID_t>(db.find_user_id(nickname)));
        res.information.append_val(static_cast<std::string>(nickname));
    }
    return res;
}

std::optional<TaskOutgoing> TaskManager::processSendMessageByName(const TaskIncoming &task) {
    std::string recipient = std::get<std::string>(task.information.data[0]);
    std::string message = std::get<std::string>(task.information.data[1]);

    int64_t recipientId = db.find_user_id(recipient);
    int64_t messageId = db.update_message(*task.author.userId, recipientId, message);

    TaskOutgoing res;
    ClientData data1;
    data1.userId = recipientId; //redundant
    data1.username = recipient;

    ClientData data2 = task.author;
    //data2.userId = task.author.userId;
    //data2.username = task.author.username;
    //data2.connectionId = task.author.connectionId;
    //todo: verify if thats an improvement


    res.recipients.push_back(data1);
    res.recipients.push_back(data2);
    res.information.opcode = RELAY_MESSAGE_BY_NAME;
    res.information.append_val(*task.author.username);
    res.information.append_val(static_cast<ID_t>(messageId));
    res.information.append_val(message);
    return res;
}

std::optional<TaskOutgoing> TaskManager::processGetLatestMessagesByName(const TaskIncoming &task) {
    std::string recipient = std::get<std::string>(task.information.data[0]);
    int64_t authorID = *task.author.userId;
    int64_t recipientId = db.find_user_id(recipient);

    std::vector<Message> mess = db.get_latest_messages(authorID, recipientId);

    std::println(std::cout, "Number of messages: {} [processGetLatestMessagesByName]", mess.size());


    TaskOutgoing res;
    ClientData data = task.author;
    res.recipients.push_back(std::move(data));

    res.information.opcode = RELAY_LATEST_MESSAGES_BY_NAME;
    res.information.append_val(recipient);
    res.information.append_val(static_cast<std::uint16_t>(mess.size()));
    for (const auto &message: mess) {
        std::println(std::cout, "Message ID: {} [processGetLatestMessagesByName]", static_cast<ID_t>(message.message_id));
        std::println(std::cout, "Message author bool: {} [processGetLatestMessagesByName]", static_cast<Byte>(message.outgoing));
        std::println(std::cout, "Message content: {} [processGetLatestMessagesByName]", message.content);
        res.information.append_val(static_cast<ID_t>(message.message_id));
        res.information.append_val(static_cast<Byte>(message.outgoing));
        res.information.append_val(message.content);
    }
    return res;


}





