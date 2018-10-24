cmake_minimum_required(VERSION 3.7)
project(TrickyTowerServer)

set(CMAKE_CXX_STANDARD 14)


LINK_DIRECTORIES(/usr/local/lib)

add_executable(Websocket main.cpp server.h room.cpp room.h semaphore.cpp semaphore.h websocket.cpp websocket.h)

target_link_libraries(Websocket libcrypto.so)

target_link_libraries(Websocket libcjson.so)

