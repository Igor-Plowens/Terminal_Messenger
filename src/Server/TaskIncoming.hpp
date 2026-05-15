#pragma once
#include "Client.hpp"
#include "InformationUnit/InformationUnit.hpp"

struct TaskIncoming {
    TaskIncoming(ClientData data, InformationUnit &&unit): author(data), information(std::move(unit)) {}

    ClientData author;
    InformationUnit information;
};


struct TaskOutgoing {
    std::vector<ClientData> recipients;
    InformationUnit information;
};



