cmake_minimum_required(VERSION 4.1)
project(messenger_clion)

find_package(SQLiteCpp REQUIRED)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)


add_executable(server
        src/Server/TaskIncoming.hpp
        src/Server/Connection/Connection.cpp
        src/Server/Connection/Connection.hpp
        src/Server/Client.hpp
        src/Server/EpollManager/EpollManager.cpp
        src/Server/EpollManager/EpollManager.hpp
        src/Shared/InformationUnit/InformationUnit.hpp
        src/Shared/Parsing/parsing.cpp
        src/Shared/Parsing/parsing.hpp
        src/Shared/includes.hpp
        src/Server/Overseer/NetworkThread/NetworkThread.cpp
        src/Server/Overseer/NetworkThread/NetworkThread.hpp
        src/Server/Overseer/TaskThread/TaskThread.cpp
        src/Server/Overseer/TaskThread/TaskThread.hpp
        src/Server/Database/Database.hpp
        src/Server/Database/Database.cpp
        src/Server/Overseer/Overseer.cpp
        src/Server/Overseer/Overseer.hpp
        src/Server/main.cpp
)

add_executable(client
        src/Client/client_main.cpp
        src/Client/Connection/Connection.cpp
        src/Client/Connection/Connection.hpp
        src/Client/Queue/Queue.hpp
        src/Client/client_main.cpp
        src/Client/includesFTXUI.hpp
        src/Shared/InformationUnit/InformationUnit.hpp
        src/Shared/Parsing/parsing.cpp
        src/Shared/Parsing/parsing.hpp
        src/Shared/includes.hpp
        src/Client/UI/LoginPage/LoginPage.hpp
        src/Client/UI/LoginPage/LoginPage.cpp
        src/Client/UI/RegisterPage/RegisterPage.cpp
        src/Client/UI/RegisterPage/RegisterPage.hpp
        src/Client/UI/StartingPage/StartingPage.cpp
        src/Client/UI/StartingPage/StartingPage.hpp
        src/Client/UI/PageManager/PageManager.cpp
        src/Client/UI/PageManager/PageManager.hpp
        src/Client/Overseer/Overseer.cpp
        src/Client/Overseer/Overseer.hpp
        src/Client/UI/MenuPage/MenuPage.cpp
        src/Client/UI/MenuPage/MenuPage.hpp
)

target_include_directories(server
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Shared
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Server
)

target_include_directories(client
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Shared
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Client
)

target_compile_definitions(server PRIVATE SERVER)
target_compile_definitions(client PRIVATE CLIENT)


include_directories(/usr/include)
target_link_libraries(server PRIVATE SQLiteCpp sqlite3)
target_link_libraries(client
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)
