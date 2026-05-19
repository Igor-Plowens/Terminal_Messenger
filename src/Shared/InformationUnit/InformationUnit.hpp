#pragma once

#pragma once
#include "../includes.hpp"

#include <vector>
#include <variant>
#include <string>

enum Opcode: std::uint16_t{
    LOGIN,
    LOGIN_SUCCESS,
    LOGIN_FAIL,
    REGISTER,
    REGISTER_SUCCESS,
    REGISTER_FAILURE,
    SEND_MESSAGE_BY_NAME,
    RELAY_MESSAGE,
    RELAY_MESSAGE_BY_NAME,
    GET_FURTHER_MESSAGES,
    GET_FURTHER_MESSAGES_BY_NAME,
    GET_LATEST_MESSAGES,
    GET_LATEST_MESSAGES_BY_NAME,
    RELAY_FURTHER_MESSAGES,
    RELAY_LATEST_MESSAGES,
    RELAY_LATEST_MESSAGES_BY_NAME,
    RELAY_FURTHER_MESSAGES_BY_NAME,

    ERROR_DESTINATION_RECIPIENT_INVALID,
    BULLSHIT = 67
};




using Payload = std::vector<std::variant<std::string, Byte, std::uint16_t, std::uint64_t>>;

class InformationUnit{
public:
    void append_val(std::string &&str){
        size += 2 + str.length();
        data.emplace_back(std::move(str));
    }

    void append_val(const std::string &str){
        size += 2 + str.length();
        data.push_back(str);
    }

    void append_val(Byte byte){
        size += 1;
        data.emplace_back(byte);
    }
    void append_val(std::uint16_t val){
        size += 2;
        data.emplace_back(val);
    }
    void append_val(std::uint64_t id){
        size += 8;
        data.emplace_back(id);
    }


    [[nodiscard]] Size get_size() const{
        return size;
    }
    Opcode opcode = BULLSHIT;
    Payload data;
private:
    Size size = 2;
};



