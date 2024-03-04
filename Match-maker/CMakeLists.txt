cmake_minimum_required(VERSION 3.26)
project(Match_maker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

find_package(Qt6Sql REQUIRED)

add_executable(Match_maker main.cpp
        main_wnd.cpp
        main_wnd.h
        main_wnd.ui
        add_user_dialog.cpp
        add_user_dialog.h
        add_user_dialog.ui
        UsersDB.cpp
        UsersDB.h
        Player.cpp
        Player.h
        MatchMaker.cpp
        MatchMaker.h


)
target_link_libraries(Match_maker
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Sql
)
