#pragma once
#include "includes.hpp"
#include <vector>
#include <variant>
#include <string>
#include <functional>

using Payload = std::vector<std::variant<std::string, Byte, std::uint16_t, std::uint64_t>>;

struct UiState;
class Networker;


using Task = std::function<void(UiState&, Networker&)>;

// struct Task {
//
//     std::function<void(UiState&, Networker&)> callback;
//     Payload data;
// };

