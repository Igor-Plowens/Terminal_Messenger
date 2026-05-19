#include "parsing.hpp"
#include <iostream>
#include <map>


#ifdef SERVER


const std::map<Opcode, std::vector<DataType>> signatures_incoming({
        {LOGIN, {STRING, STRING}},
        {REGISTER, {STRING, STRING}},
        {SEND_MESSAGE_BY_NAME, {STRING, STRING}},
        {GET_FURTHER_MESSAGES, {ID, ID}},
        {GET_FURTHER_MESSAGES_BY_NAME, {STRING, ID}},
        {GET_LATEST_MESSAGES, {ID}},
        {GET_LATEST_MESSAGES_BY_NAME, {STRING}}
    });


const std::map<Opcode, std::vector<DataType>> signatures_incoming_extended{};


#elifdef CLIENT


const std::map<Opcode, std::vector<DataType>> signatures_incoming({
        {LOGIN_SUCCESS, {ID, STRING}},
        {LOGIN_FAIL, {}},
        {REGISTER_SUCCESS, {ID, STRING}},
        {REGISTER_FAILURE, {}},
        {RELAY_MESSAGE, {ID, ID, STRING}},
        {RELAY_MESSAGE_BY_NAME, {STRING, ID, STRING}},
        {RELAY_FURTHER_MESSAGES, {ID, UINT16}},
        {RELAY_FURTHER_MESSAGES_BY_NAME, {STRING, UINT16}},
        {RELAY_LATEST_MESSAGES, {ID, UINT16}},
        {RELAY_LATEST_MESSAGES_BY_NAME, {STRING, UINT16}},
    });


const std::map<Opcode, std::vector<DataType>> signatures_incoming_extended{
        {RELAY_FURTHER_MESSAGES, {ID, BYTE, STRING}},
        {RELAY_FURTHER_MESSAGES_BY_NAME, {ID, BYTE, STRING}},
        {RELAY_LATEST_MESSAGES_BY_NAME, {ID, BYTE, STRING}}
};



#endif



InformationUnit Parsing::parse_buffer(const Buffer &buff){
    InformationUnit res;
    const Byte *buffer_iter = buff.data();
    res.opcode = static_cast<Opcode>(parse_int<std::uint16_t>(buffer_iter));
    for (const auto ref: signatures_incoming.at(res.opcode)) {
        switch (ref) {
            case BYTE:
                res.append_val(parse_int<Byte>(buffer_iter));
                break;
            case UINT16:
                res.append_val(parse_int<std::uint16_t>(buffer_iter));
                break;
            case STRING:
                res.append_val(parse_string(buffer_iter));
                break;
            case ID:
                res.append_val(parse_int<std::uint64_t>(buffer_iter));
                break;
            default:
                throw std::runtime_error(std::string("Unknown error at line: ") + std::to_string(__LINE__));
        }
    }

    if (signatures_incoming_extended.find(res.opcode) != signatures_incoming_extended.end()){
        std::uint16_t number_of_elems = std::get<std::uint16_t>(res.data[1]);
        for (std::uint16_t i = 0; i < number_of_elems; ++i){
            for (const auto &ref: signatures_incoming_extended.at(res.opcode)){
                switch (ref){
                    case BYTE:
                        res.append_val(parse_int<Byte>(buffer_iter));
                        break;
                    case UINT16:
                        res.append_val(parse_int<std::uint16_t>(buffer_iter));
                        break;
                    case STRING:
                        res.append_val(parse_string(buffer_iter));
                        break;
                    case ID:
                        res.append_val(parse_int<std::uint64_t>(buffer_iter));
                        break;
                    default:
                        throw std::runtime_error(std::string("Unknown error at line: ") + std::to_string(__LINE__));
                }
            }
        }
    }
    return res;

}

std::vector<Byte> Parsing::imprint_buffer(const InformationUnit &arg){
    std::vector<Byte> res;
    res.resize(arg.get_size() + 4);
    Byte *starting_ptr = res.data();
    Byte *ptr_iter = starting_ptr;
    imprint_int<std::uint32_t>(ptr_iter, arg.get_size());
    imprint_int<std::uint16_t>(ptr_iter, arg.opcode);

    for (auto &ref: arg.data){
        if (std::holds_alternative<Byte>(ref)){
            imprint_int<Byte>(ptr_iter, std::get<Byte>(ref));
        }
        else if (std::holds_alternative<std::uint16_t>(ref)){
            imprint_int<std::uint16_t>(ptr_iter, std::get<std::uint16_t>(ref));
        }
        else if (std::holds_alternative<std::uint64_t>(ref)){
            imprint_int<std::uint64_t>(ptr_iter, std::get<std::uint64_t>(ref));
        }
        else if (std::holds_alternative<std::string>(ref)){
            imprint_string(ptr_iter, std::get<std::string>(ref));
        }
        else{
            throw std::runtime_error(std::string("Invalid variant state, line: ") + std::to_string(__LINE__));
        }
    }
    return res;
}