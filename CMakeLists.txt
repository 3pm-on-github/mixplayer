cmake_minimum_required(VERSION 3.16)
project(mixplayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_add_resources(RESOURCES resources/resources.qrc)

qt_add_executable(mixplayer
    src/main.cpp
    src/DiscLabel.cpp
    ${RESOURCES}
)
target_link_libraries(mixplayer PRIVATE Qt6::Widgets Qt6::Multimedia)