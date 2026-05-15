#pragma once


#include <cstdint>
#include <string>
#include <optional>
#include <stdexcept>




#include "Connection/Connection.hpp"
#include "includes.hpp"


struct ClientData {

    bool operator<(const ClientData &other) const noexcept {
        return connectionId < other.connectionId;
    }

    bool operator==(const ClientData &other) const noexcept {
        if (userId.has_value() && other.userId.has_value()) {
            return *userId == *other.userId;
        }
        if (username.has_value() && other.username.has_value()) {
            return *username == *other.username;
        }
        return connectionId == other.connectionId;
    }

    std::optional<std::string> username = std::nullopt;
    std::optional<ID_t> userId = std::nullopt;
    int connectionId;
};


class Client{
public:

    explicit Client(int connectionId) noexcept: conn(connectionId) {
        data.connectionId = connectionId;
    }




    void setUserInfo(ID_t id, const std::string &name){
        data.userId = id;
        data.username = name;
    }

    bool isInitialized() const noexcept{
        return data.userId.has_value();
    }


    ClientData getData() const noexcept {
        return data;
    }


    Connection conn;
private:
    ClientData data;
};









