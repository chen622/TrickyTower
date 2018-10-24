//
// Created by chen on 2018/10/22.
//

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include "room.h"
#include "websocket.h"
#include <string>
//#include "semaphore.h"

using namespace std;

#include <map>

struct player {
    int room_id = -1;
    int event = -1;//1表示房间请求
};

void broadcast(int epoll_fd);

#endif //WEBSOCKET_SERVER_H
