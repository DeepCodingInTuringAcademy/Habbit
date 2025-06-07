# CMakeLists.txt Main

cmake_minimum_required(VERSION 3.30)
project(Habbit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

include_directories(src)

file(GLOB HABBIT_HEADERS
    "src/*.h"
)

file(GLOB HABBIT_SOURCE
    "src/*.cpp"
)

add_executable(Habbit main.cpp ${HABBIT_HEADERS} ${HABBIT_SOURCE})

target_include_directories(Habbit PRIVATE src)

target_link_libraries(Habbit PRIVATE Qt6::Widgets Qt6::Sql)