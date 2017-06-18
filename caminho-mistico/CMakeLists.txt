cmake_minimum_required(VERSION 2.8)

#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 11)

project(caminho_mistico)

set(SOURCE_FILES src/main.cpp)
add_executable(caminho_mistico ${SOURCE_FILES})