#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

#include <cstdint>
#include <vector>


struct UserInfo{
    std::string name;
    std::int64_t id;
};

struct FriendRequest{
    std::string name;
    std::int64_t friendship_request_id;
    bool outgoing;
};

struct Message{
    std::int64_t message_id;
    std::string content;
    bool outgoing;
};



class Database{
public:
    Database(const char *name = "database.db");


    bool check_if_user_exists(const std::string &name);
    bool check_if_user_credentials_match(const std::string &name, const std::string &password);
    void add_user(const std::string &name, const std::string &password);


    std::int64_t update_message(std::int64_t authorId, std::int64_t destationId, const std::string &content);

    std::vector<Message> get_message_batch(std::int64_t askerID, std::int64_t interlocutor_id, std::int64_t last_message_id, std::uint64_t amount = 10);
    std::vector<Message> get_latest_messages(std::int64_t askerID, std::int64_t interlocutor_id, std::uint64_t amount = 10);

    std::int64_t find_user_id(const std::string &name);
    std::string find_users_name(std::int64_t id);

private:

    SQLite::Database db;
};