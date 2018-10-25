#include "server.h"


map<int, room> mapRoom;
map<int, player> mapPlayer;
int roomId = 60;

//scp -r C:\Users\chen\CLionProjects\Websocket root@127.0.0.1:\home\chen\Experiment\

/*
 * Request json:{
 *      function: //1:创建房间; 2:获取房间; 3:加入房间; 4:开始游戏
 *      room:{
 *          id:
 *          name:
 *      }
 * }
 */
int processRequest(char *request, epoll_event event, int epoll_fd, int i) {
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
                send_msg(event.data.fd, cJSON_PrintUnformatted(response));
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
                int list[3];
                player *host = &mapPlayer[event.data.fd];
                host->playing = 1;
                list[0] = event.data.fd;
                mapPlayer[mapRoom[host->room_id].getPlayer2()].playing = 1;
                list[1] = mapRoom[host->room_id].getPlayer2();
                mapPlayer[mapRoom[host->room_id].getPlayer3()].playing = 1;
                list[2] = mapRoom[host->room_id].getPlayer3();

                printf("player:%d,%d,%d\n",list[0],list[1],list[2]);
                for (int j = 0; j < 3; ++j) {
                    epoll_event newEvent;
                    newEvent.events = EPOLLOUT;//表示对应的文件描述符可写（包括对端SOCKET正常关闭）
                    newEvent.data.fd = list[j];//将connFd设置为要读取的文件描述符
                    mapPlayer[list[j]].event = 2;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, list[j], &newEvent) == -1) {
                        perror("epoll_ctl:conn_fd register failed");
                        exit(EXIT_FAILURE);
                    }
                }
                return 4;
            }
            default:
                return 0;
        }
    }
    return -1;
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
//                send_frame_head(events[i].data.fd, &head);
                //read payload data
                char payload_data[1024] = {0};
                int size = 0;
                do {
                    int rul;
                    rul = read(events[i].data.fd, payload_data, 1024);
                    if (rul <= 0)
                        break;
                    size += rul;

                    umask(payload_data, size, head.masking_key);

//                    //echo data
//                    if (write(events[i].data.fd, payload_data, rul) <= 0)
//                        break;
                } while (size < head.payload_length);
                if (head.opcode == 0x8) {
                    printf("socket %d close\n", events[i].data.fd, payload_data, 1024);
                    mapPlayer.erase(events[i].data.fd);
                    close(events[i].data.fd);
                }
                printf("receive data(%d):%s\n", head.payload_length, payload_data);
                processRequest(payload_data, events[i], epoll_fd, i);

            } else {
                printf("write\n");
                cJSON *response = cJSON_CreateObject();

                if (mapPlayer[events[i].data.fd].event == 1) {
                    cJSON_AddNumberToObject(response, "function", 1);
                    cJSON_AddNumberToObject(response, "type", 2);
                    cJSON_AddNumberToObject(response, "socketId", events[i].data.fd);
                    cJSON *data = cJSON_CreateArray();
                    for (map<int, room>::iterator iter = mapRoom.begin(); iter != mapRoom.end(); iter++) {
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

                    send_msg(events[i].data.fd, cJSON_PrintUnformatted(response));

                    mapPlayer[events[i].data.fd].event = -1;
                    event.events = EPOLLIN;        //表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                    event.data.fd = events[i].data.fd;//将connFd设置为要读取的文件描述符
                    //event.data.ptr = &head;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                        perror("epoll_ctl:conn_fd register failed");
                        exit(EXIT_FAILURE);
                    }
                } else if (mapPlayer[events[i].data.fd].event == 2) {
                    cJSON_AddNumberToObject(response, "function", 2);
                    cJSON_AddNumberToObject(response, "type", 1);
                    send_msg(events[i].data.fd, cJSON_PrintUnformatted(response));

                    mapPlayer[events[i].data.fd].event = -1;
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

