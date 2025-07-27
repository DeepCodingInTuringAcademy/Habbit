# CMakeLists.txt Main

cmake_minimum_required(VERSION 3.30)
project(Habbit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(src)

file(GLOB HABBIT_HEADERS
    "src/*.h"
)

file(GLOB HABBIT_SOURCE
    "src/*.cpp"
)

set(main src/main/main.cpp)
set(winWidget src/main/winWidget.cpp)

add_executable(Habbit ${main} ${HABBIT_HEADERS} ${HABBIT_SOURCE})
add_executable(HabbitWinWidget ${winWidget} ${HABBIT_HEADERS} ${HABBIT_SOURCE})

target_include_directories(Habbit PRIVATE src)
target_include_directories(HabbitWinWidget PRIVATE src)

qt_add_resources(APP_RESOURCES assets/resources.qrc)
qt_add_resources(THEMES themes/themes.qrc)
target_sources(Habbit PRIVATE ${APP_RESOURCES} ${THEMES})

target_link_libraries(Habbit PRIVATE Qt6::Widgets Qt6::Sql)
target_link_libraries(HabbitWinWidget PRIVATE Qt6::Widgets Qt6::Sql)