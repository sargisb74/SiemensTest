cmake_minimum_required(VERSION 3.26)
project(XOGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

add_executable(XOGame main.cpp)
target_link_libraries(XOGame
        Qt::Core
)

