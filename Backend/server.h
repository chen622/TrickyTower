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
    int event = -1;//1表示房间请求;2表示开始;3消息转发
    cJSON *msg1 = NULL;
    cJSON *msg2 = NULL;
    int playing = 0;
};

void broadcast(int epoll_fd);
void roomcast(int epoll_fd,int socketFd,int eventId,cJSON *data);
void deleteFromEpoll(int epoll_fd,int socketFd);

#endif //WEBSOCKET_SERVER_H
