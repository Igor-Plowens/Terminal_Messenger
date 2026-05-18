
#include "Database.hpp"
#include <algorithm>


Database::Database(const char *name): db(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE){
    db.exec(
        "CREATE TABLE IF NOT EXISTS Users ("
        "User_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Username TEXT UNIQUE,"
        "Password TEXT);"
    );

    db.exec(
        "CREATE TABLE IF NOT EXISTS Friendships ("
        "Friendship_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "User1_ID INTEGER,"
        "User2_ID INTEGER);"
    );

    // db.exec(
    //     "CREATE TABLE IF NOT EXISTS Friend_Requests ("
    //     "Friend_request_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
    //     "Sender_ID INTEGER,"
    //     "Receiver_ID INTEGER);"
    // );


    db.exec(
        "CREATE TABLE IF NOT EXISTS Messages ("
        "Message_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Sender_ID INTEGER,"
        "Receiver_ID INTEGER,"
        "Content TEXT);"
    );

}


bool Database::check_if_user_exists(const std::string &username){
    SQLite::Statement query(
        db,
        "SELECT User_ID FROM Users WHERE Username = ?;"
    );
    query.bind(1, username);
    return query.executeStep();
}

bool Database::check_if_user_credentials_match(const std::string &username, const std::string &password){
    SQLite::Statement query(
        db,
        "SELECT User_ID FROM Users WHERE Username = ? AND Password = ?;"
    );
    query.bind(1, username);
    query.bind(2, password);
    return query.executeStep();
}

std::int64_t Database::find_user_id(const std::string &username){
    SQLite::Statement query(
        db,
        "SELECT User_ID FROM Users WHERE Username = ?;"
    );
    query.bind(1, username);
    if (!query.executeStep()){
        throw std::runtime_error("Attempted finding a user that doesnt exist");
    }
    return query.getColumn(0).getInt64();
}

std::string Database::find_users_name(std::int64_t arg){
    SQLite::Statement query(
        db,
        "SELECT Username FROM Users WHERE User_ID = ?;"
    );
    query.bind(1, arg);
    if (!query.executeStep()){
        throw std::runtime_error("Attempted finding a user that doesnt exist");
    }
    return query.getColumn(0).getText();
}

void Database::add_user(const std::string &name, const std::string &password){
    SQLite::Statement statement(
        db,
        "INSERT INTO Users "
        "(Username, Password) "
        "VALUES "
        "(?, ?)"
    );

    statement.bind(1, name);
    statement.bind(2, password);
    try{
        statement.exec();
    }
    catch(...){
        throw std::runtime_error("Error: couldnt add a user because he exists");
    }
}



std::int64_t Database::update_message(std::int64_t authorID, std::int64_t destationID, const std::string &content){
    SQLite::Statement statement(
        db,
        "INSERT INTO Messages "
        "(Sender_ID, Receiver_ID, Content) "
        "VALUES "
        "(?, ?, ?);"
    );
    statement.bind(1, authorID);
    statement.bind(2, destationID);
    statement.bind(3, content);
    statement.exec();


    SQLite::Statement query(
        db,
        "SELECT Message_ID "
        "FROM Messages "
        "WHERE Sender_ID = ? AND Receiver_ID = ? AND Content = ?;"
    );
    query.bind(1, authorID);
    query.bind(2, destationID);
    query.bind(3, content);
    query.executeStep();
    return query.getColumn(0).getInt64();
}

std::vector<Message> Database::get_message_batch(std::int64_t askerID, std::int64_t interlocutor_id,
    std::int64_t last_message_id, std::uint64_t amount){

    std::int64_t higher_bound = last_message_id - 1;
    std::int64_t lower_bound = higher_bound - amount + 1;


    SQLite::Statement query(
        db,
        "SELECT Message_ID, Content "
        "FROM Messages "
        "WHERE (Sender_ID = ?) AND (Receiver_ID = ?) AND (Message_ID BETWEEN ? AND ?);"
    );
    query.bind(1, askerID);
    query.bind(2, interlocutor_id);
    query.bind(3, lower_bound);
    query.bind(4, higher_bound);

    std::vector<Message> messages;
    while (query.executeStep()){
        messages.push_back({
            query.getColumn(0).getInt64(),
            query.getColumn(1).getText(),
            true
        });
    }


    SQLite::Statement query2(
        db,
        "SELECT Message_ID, Content "
        "FROM Messages "
        "WHERE (Receiver_ID = ?) AND (Sender_ID = ?) AND (Message_ID BETWEEN ? AND ?);"
    );
    query2.bind(1, askerID);
    query2.bind(2, interlocutor_id);
    query2.bind(3, lower_bound);
    query2.bind(4, higher_bound);
    while (query2.executeStep()){
        messages.push_back({
            query2.getColumn(0).getInt64(),
            query2.getColumn(1).getText(),
            false
        });
    }
    std::sort(
        messages.begin(),
        messages.end(),
        [](const Message &mess1, const Message &mess2){
            return mess1.message_id < mess2.message_id;
        }
    );
    return messages;
}

std::vector<Message> Database::get_latest_messages(std::int64_t askerID, std::int64_t interlocutor_id, std::uint64_t amount){
    SQLite::Statement query(
        db,
        "SELECT Message_ID, Sender_ID, Content FROM Messages "
        "WHERE ((Sender_ID = ? AND Receiver_ID = ?) OR (Sender_ID = ? AND Receiver_ID = ?)) "
        "ORDER BY Message_ID DESC "
        "LIMIT ? "
    );

    query.bind(1, askerID);
    query.bind(2, interlocutor_id);
    query.bind(3, interlocutor_id);
    query.bind(4, askerID);
    query.bind(5, static_cast<std::int64_t>(amount));
    std::vector<std::int64_t> authors;
    std::vector<Message> messages;
    while (query.executeStep()){
        messages.emplace_back(
            query.getColumn(0).getInt64(),
            query.getColumn(2).getText(),
            true
        );
        authors.push_back(query.getColumn(1).getInt64());
    }
    for (int i = 0; i < authors.size(); ++i) {
        if (authors[i] != askerID) {
            messages[i].outgoing = false;
        }
    }

    std::ranges::sort(
        messages,
        [](const Message &mess1, const Message &mess2){
            return mess1.message_id > mess2.message_id;
        }
    );
    return messages;

}