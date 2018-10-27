#include "server.h"


map<int, room> mapRoom;
map<int, player> mapPlayer;
int roomId = 60;

//scp -r C:\Users\chen\CLionProjects\Websocket root@127.0.0.1:\home\chen\Experiment\

/*
 * Request json:{
 *      function: //1:创建房间; 2:获取房间; 3:加入房间; 4:开始游戏; 5:心跳同步
 *      room:{
 *          id:
 *          name:
 *      }，
 *      bodies:{
 *      }
 * }
 */
int processRequest(char *request, epoll_event event, int epoll_fd, int i) {
    printf("process\n");
    cJSON *data = cJSON_Parse(request);
    cJSON *response = cJSON_CreateObject();
    if (!data) {
        printf("Error before:[%s]\n", cJSON_GetErrorPtr());
    } else {
        int function = cJSON_GetObjectItem(data, "function")->valueint;
        printf("function:%d\n", function);
        switch (function) {
            case 1: {//创建房间
//                if (mapPlayer[event.data.fd].room_id==-1){
//                    cJSON_AddNumberToObject(response)
//                }
                string name = cJSON_GetStringValue(cJSON_GetObjectItem(cJSON_GetObjectItem(data, "room"), "name"));
                room new_room(name, event.data.fd);
                printf("1\n");
                mapRoom[roomId] = new_room;
                mapPlayer[event.data.fd].room_id = roomId;
                cJSON_AddNumberToObject(response, "function", 1);
                cJSON_AddNumberToObject(response, "type", 1);
                cJSON_AddNumberToObject(response, "roomId", roomId);
                if (send_msg(event.data.fd, cJSON_PrintUnformatted(response)) == -1) {
                    deleteFromEpoll(epoll_fd, event.data.fd);
                    return -1;
                }
                roomId++;
                broadcast(epoll_fd);
                return 1;
            };
            case 2: {
                broadcast(epoll_fd);
                return 2;
            };
            case 3: {
                int roomId = cJSON_GetObjectItem(cJSON_GetObjectItem(data, "room"), "id")->valueint;
                mapRoom[roomId].addPlayer(event.data.fd);
                mapPlayer[event.data.fd].room_id = roomId;
                broadcast(epoll_fd);
                return 3;
            };
            case 4: {
                roomcast(epoll_fd, event.data.fd, 2, data);
                return 4;
            };
            case 5: {
                roomcast(epoll_fd, event.data.fd, 3, data);
                return 5;
            }
            default:
                return 0;
        }
    }
    return -1;
}

void roomcast(int epoll_fd, int socketFd, int eventId, cJSON *data) {
    int list[3];
    int size = 0;
    if (eventId == 2) {
        size = 3;
        player *host = &mapPlayer[socketFd];
        host->playing = 1;
        list[0] = socketFd;
        list[1] = mapRoom[host->room_id].getPlayer2();
        list[2] = mapRoom[host->room_id].getPlayer3();
        mapPlayer[mapRoom[host->room_id].getPlayer2()].playing = 1;
        mapPlayer[mapRoom[host->room_id].getPlayer3()].playing = 1;
    } else {
        room currentRoom = mapRoom[mapPlayer[socketFd].room_id];
        size = 2;
        if (currentRoom.getHost() == socketFd) {
            list[0] = currentRoom.getPlayer2();
            if (mapPlayer[list[0]].msg1 != NULL)
                printf("1\n");
            mapPlayer[list[0]].msg1 = cJSON_GetObjectItem(data, "bodies");
            list[1] = currentRoom.getPlayer3();
            if (mapPlayer[list[1]].msg1 != NULL)
                printf("2\n");
            mapPlayer[list[1]].msg1 = cJSON_GetObjectItem(data, "bodies");
        } else if (currentRoom.getPlayer2() == socketFd) {
            list[0] = currentRoom.getHost();
            if (mapPlayer[list[0]].msg1 != NULL)
                printf("3\n");
            mapPlayer[list[0]].msg1 = cJSON_GetObjectItem(data, "bodies");
            list[1] = currentRoom.getPlayer3();
            if (mapPlayer[list[1]].msg2 != NULL)
                printf("4\n");
            mapPlayer[list[1]].msg2 = cJSON_GetObjectItem(data, "bodies");
        } else if (currentRoom.getPlayer3() == socketFd) {
            list[0] = currentRoom.getHost();
            if (mapPlayer[list[0]].msg2 != NULL)
                printf("5\n");
            mapPlayer[list[0]].msg2 = cJSON_GetObjectItem(data, "bodies");
            list[1] = currentRoom.getPlayer2();
            if (mapPlayer[list[1]].msg2 != NULL)
                printf("6\n");
            mapPlayer[list[1]].msg2 = cJSON_GetObjectItem(data, "bodies");
        }
    }

    printf("player:%d,%d,%d\n", list[0], list[1], list[2]);
    for (int j = 0; j < size; ++j) {
        epoll_event newEvent;
        newEvent.events = EPOLLOUT;//表示对应的文件描述符可写（包括对端SOCKET正常关闭）
        newEvent.data.fd = list[j];//将connFd设置为要读取的文件描述符
        mapPlayer[list[j]].event = eventId;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, list[j], &newEvent) == -1) {
            perror("epoll_ctl:conn_fd register failed");
            exit(EXIT_FAILURE);
        }
    }
}

void broadcast(int epoll_fd) {
    for (map<int, player>::iterator iter = mapPlayer.begin(); iter != mapPlayer.end(); iter++) {
        if (iter->second.playing == 1) {
            continue;
        }
        epoll_event newEvent;
        newEvent.events = EPOLLOUT;        //表示对应的文件描述符可写（包括对端SOCKET正常关闭）
        newEvent.data.fd = iter->first;//将connFd设置为要读取的文件描述符
        iter->second.event = 1;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, iter->first, &newEvent) == -1) {
            perror("epoll_ctl:conn_fd register failed");
            exit(EXIT_FAILURE);
        }
    }
}

void deleteFromEpoll(int epoll_fd, int socketFd) {
	printf("write:end of socket:%d\n",socketFd);
    int roomId = mapPlayer[socketFd].room_id;
    if (roomId != -1) {
        room *current = &mapRoom[roomId];
        if (current->getHost() == socketFd) {
            mapRoom.erase(roomId);
        } else if (current->getPlayer2() == socketFd) {
            current->setPlayer2(-1);
            current->setConnectAmount(current->getConnectAmount() - 1);
        } else if (current->getPlayer3() == socketFd) {
            current->setPlayer3(-1);
            current->setConnectAmount(current->getConnectAmount() - 1);
        }
    }
    mapPlayer.erase(socketFd);
    broadcast(epoll_fd);
    close(socketFd);
}

int main() {
    int listen_fd = passive_server(8008, 20);
    int connFd;

    struct sockaddr_in client_addr;
    socklen_t addr_length = sizeof(client_addr);

    int epoll_fd = epoll_create(MAX_EVENTS);

    if (epoll_fd == -1) {
        perror("epoll_create failed\n");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    event.events = EPOLLIN;
    event.data.fd = listen_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1)    //注册新的listen_fd到epoll_fd中
    {
        perror("epll_ctl:servaddr register failed\n");
        exit(EXIT_FAILURE);
    }
    printf("listen:%d\n", listen_fd);
    int nfds;
    while (true) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
        if (nfds != 0)
            printf("\nnfds:%d\n", nfds);
        if (nfds == -1) {
            perror("start epoll_wait failed\n");
            continue;
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listen_fd) {
                if ((connFd = accept(listen_fd, (struct sockaddr *) &client_addr, &addr_length)) < 0) {
                    perror("accept conn_fd failed");
                    exit(EXIT_FAILURE);
                }
                printf("accept:%d\n", connFd);
                shakehands(connFd);
                printf("shakerhands\n");
                player newPlayer;
                mapPlayer[connFd] = newPlayer;

                event.events = EPOLLOUT;//表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                mapPlayer[connFd].event = 1;
                event.data.fd = connFd;//将connFd设置为要读取的文件描述符
                //event.data.ptr = &head;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connFd, &event) == -1) {
                    perror("epoll_ctl:conn_fd register failed");
                    exit(EXIT_FAILURE);
                }
            } else if (events[i].events & EPOLLIN) {
                printf("receive\n");
                frame_head head;
                int rul = recv_frame_head(events[i].data.fd, &head);
                if (rul < 0) {
                    close(events[i].data.fd);
                    break;
                }
//                send_frame_head(socketFd, &head);
                //read payload data
                char payload_data[102400] = {0};
                int size = 0;
                do {
                    int rul;
                    rul = read(events[i].data.fd, payload_data, 1024);
                    if (rul <= 0)
                        break;
                    size += rul;

                    umask(payload_data, size, head.masking_key);

//                    //echo data
//                    if (write(socketFd, payload_data, rul) <= 0)
//                        break;
                } while (size < head.payload_length);
                if (head.opcode == 0x8) {
                    printf("socket %d close\n", events[i].data.fd, payload_data, 1024);
                    deleteFromEpoll(epoll_fd, events[i].data.fd);
                }
                //printf("receive data(%d):%s\n", head.payload_length, payload_data);
                processRequest(payload_data, events[i], epoll_fd, i);

            } else {
                printf("write\n");
                cJSON *response = cJSON_CreateObject();

                if (mapPlayer[events[i].data.fd].event == 1) {//广播所有房间
                    cJSON_AddNumberToObject(response, "function", 1);
                    cJSON_AddNumberToObject(response, "type", 2);
                    cJSON_AddNumberToObject(response, "socketId", events[i].data.fd);
                    cJSON *data = cJSON_CreateArray();
                    for (map<int, room>::iterator iter = mapRoom.begin(); iter != mapRoom.end(); iter++) {
                        while (iter->second.getConnectAmount() == 0) {
                            iter = mapRoom.erase(iter);
                            if (iter == mapRoom.end()) {
                                break;
                            }
                        }
                        cJSON *r = cJSON_CreateObject();
                        cJSON_AddNumberToObject(r, "id", iter->first);
                        cJSON_AddNumberToObject(r, "amount", iter->second.getConnectAmount());
                        cJSON_AddStringToObject(r, "name", iter->second.getName().c_str());
                        cJSON_AddNumberToObject(r, "host", iter->second.getHost());
                        cJSON_AddNumberToObject(r, "player2", iter->second.getPlayer2());
                        cJSON_AddNumberToObject(r, "player3", iter->second.getPlayer3());

                        cJSON_AddItemToArray(data, r);
                    }
                    cJSON_AddItemToObject(response, "data", data);
                    if (send_msg(events[i].data.fd, cJSON_PrintUnformatted(response)) == -1) {
                        deleteFromEpoll(epoll_fd, events[i].data.fd);
                    } else {
                        mapPlayer[events[i].data.fd].event = -1;
                        event.events = EPOLLIN;        //表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                        event.data.fd = events[i].data.fd;//将connFd设置为要读取的文件描述符
                        //event.data.ptr = &head;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                            perror("epoll_ctl:conn_fd register failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                } else if (mapPlayer[events[i].data.fd].event == 2) {//开始游戏
                    cJSON_AddNumberToObject(response, "function", 2);
                    cJSON_AddNumberToObject(response, "type", 1);
                    if (send_msg(events[i].data.fd, cJSON_PrintUnformatted(response)) == -1) {
                        deleteFromEpoll(epoll_fd, events[i].data.fd);
                    } else {
                        mapPlayer[events[i].data.fd].event = -1;
                        event.events = EPOLLIN;        //表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                        event.data.fd = events[i].data.fd;//将connFd设置为要读取的文件描述符
                        //event.data.ptr = &head;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                            perror("epoll_ctl:conn_fd register failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                } else if (mapPlayer[events[i].data.fd].event == 3) {//游戏时传递位置
                    mapPlayer[events[i].data.fd].event = -1;
                    cJSON_AddNumberToObject(response, "function", 3);
                    if (mapPlayer[events[i].data.fd].msg1 != NULL) {
                        cJSON_AddNumberToObject(response, "type", 0);
                        cJSON_AddItemToObject(response, "data", mapPlayer[events[i].data.fd].msg1);
                        mapPlayer[events[i].data.fd].msg1 = NULL;
                    } else {
                        cJSON_AddNumberToObject(response, "type", 1);
                        cJSON_AddItemToObject(response, "data", mapPlayer[events[i].data.fd].msg2);
                        mapPlayer[events[i].data.fd].msg2 = NULL;
                    }
                    if (send_msg(events[i].data.fd, cJSON_PrintUnformatted(response)) == -1) {
                        deleteFromEpoll(epoll_fd, events[i].data.fd);
                    } else {
                        event.events = EPOLLIN;        //表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                        event.data.fd = events[i].data.fd;//将connFd设置为要读取的文件描述符
                        //event.data.ptr = &head;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                            perror("epoll_ctl:conn_fd register failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }

        }
    }
}

