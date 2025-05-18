# CMakeLists.txt Main

cmake_minimum_required(VERSION 3.30)
project(Habbit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(SQLite3 REQUIRED)

include_directories(src)

file(GLOB HABBIT_HEADERS
    "src/*.h"
)

file(GLOB HABBIT_SOURCE
    "src/*.cpp"
)

add_executable(Habbit main.cpp)

target_include_directories(Habbit PRIVATE src)

target_link_libraries(Habbit ${HABBIT_HEADERS} ${HABBIT_SOURCE} Qt6::Widgets ${SQLite3_LIBRARIES})