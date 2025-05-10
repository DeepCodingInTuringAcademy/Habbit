# CMakeLists.txt Main

cmake_minimum_required(VERSION 3.30)
project(Habbit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

add_executable(Habbit main.cpp)