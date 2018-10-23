cmake_minimum_required(VERSION 3.7)
project(TrickyTowerServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(Websocket main.cpp server.h)