#pragma once


#pragma once
#include <cstdint>

#include "../InformationUnit/InformationUnit.hpp"
#include <iostream>
enum DataType {
    STRING,
    BYTE,
    UINT16,
    ID
};



namespace Parsing{

    template<class T>
    T parse_int(const Byte *&buff){
        T res{0};
        T mul = 1;
        for (int i = 0; i < sizeof(T); ++i){
            res += static_cast<std::uint64_t>(*buff++) * mul;
            mul *= 256;
        }
        return res;
    }
    template<class T>
    std::uint8_t imprint_int(Byte *&buff, T arg){
        Byte *ptr = reinterpret_cast<Byte *>(&arg);
        for (int i = 0; i < sizeof(T); ++i){
            *buff++ = *ptr++;
        }
        return sizeof(arg);
    }

    inline std::uint8_t imprint_string(Byte *&buff, const std::string &arg){
        imprint_int<StrLen>(buff, arg.size());
        const char *ptr = reinterpret_cast<const char *>(arg.data());
        for (StrLen i = 0; i < arg.size(); ++i){
            *buff++ = *ptr++;
        }
        return sizeof(std::uint16_t) + arg.length();
    }

    inline std::string parse_string(const Byte *&buff) {
        StrLen len = parse_int<StrLen>(buff);
        std::string res(reinterpret_cast<const char *>(buff), len); //original ver
        buff += res.size();
        return res;
    }

    InformationUnit parse_buffer(const Buffer &buff);
    std::vector<Byte> imprint_buffer(const InformationUnit &arg);

}