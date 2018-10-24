#include "server.h"


#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cjson/cJSON.h>
#include <sys/sem.h>


using namespace std;

map<int, room> mapRoom;
map<int, player> mapPlayer;
int roomId = 60;

//scp -r C:\Users\chen\CLionProjects\Websocket root@127.0.0.1:\home\chen\Experiment\

/*
 * Request json:{
 *      function: //1:创建房间，2:获取房间
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
                printf("sockcet:%d\n", event.data.fd);
                char name[20];
                strcpy(name, "撕逼小组");
                room new_room(name, event.data.fd);
                printf("1\n");
                mapRoom[roomId] = new_room;
                cJSON_AddNumberToObject(response, "function", 1);
                cJSON_AddNumberToObject(response, "type", 1);
                cJSON_AddNumberToObject(response, "roomId", roomId);
                char *json = cJSON_PrintUnformatted(response);
                printf("socket:%d\n", event.data.fd);
                frame_head head;
                head.payload_length = strlen(json);
                send_frame_head(event.data.fd, &head);
                write(event.data.fd, json, strlen(json));
                roomId++;
                broadcast(epoll_fd);
                return 1;
            };
            case 2: {
                broadcast(epoll_fd);
            }

            default:
                return 0;
        }
    }
    return -1;
}

void broadcast(int epoll_fd) {
    for (map<int, player>::iterator iter = mapPlayer.begin(); iter != mapPlayer.end(); iter++) {
        if (iter->second.room_id != -1) {
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

int passive_server(int port, int queue) {
    ///定义sockfd
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if (bind(server_sockfd, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    ///listen，成功返回0，出错返回-1
    if (listen(server_sockfd, queue) == -1) {
        perror("listen");
        exit(1);
    }
    printf("监听%d端口\n", port);
    return server_sockfd;
}

int base64_encode(char *in_str, int in_len, char *out_str) {
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;
    size_t size = 0;

    if (in_str == NULL || out_str == NULL)
        return -1;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in_str, in_len);
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bptr);
    memcpy(out_str, bptr->data, bptr->length);
    out_str[bptr->length - 1] = '\0';
    size = bptr->length;

    BIO_free_all(bio);
    return size;
}

/**
 * @brief _readline
 * read a line string from all buffer
 * @param allbuf
 * @param level
 * @param linebuf
 * @return
 */
int _readline(char *allbuf, int level, char *linebuf) {
    int len = strlen(allbuf);
    for (; level < len; ++level) {
        if (allbuf[level] == '\r' && allbuf[level + 1] == '\n')
            return level + 2;
        else
            *(linebuf++) = allbuf[level];
    }
    return -1;
}

int shakehands(int cli_fd) {
    //next line's point num
    int level = 0;
    //all request data
    char buffer[BUFFER_SIZE];
    //a line data
    char linebuf[256];
    //Sec-WebSocket-Accept
    char sec_accept[32];
    //sha1 data
    unsigned char sha1_data[SHA_DIGEST_LENGTH + 1] = {0};
    //reponse head buffer
    char head[BUFFER_SIZE] = {0};

    if (read(cli_fd, buffer, sizeof(buffer)) <= 0)
        perror("read");
    printf("request\n");
    printf("%s\n", buffer);

    do {
        memset(linebuf, 0, sizeof(linebuf));
        level = _readline(buffer, level, linebuf);
        //printf("line:%s\n",linebuf);

        if (strstr(linebuf, "Sec-WebSocket-Key") != NULL) {
            strcat(linebuf, GUID);
//            printf("key:%s\nlen=%d\n",linebuf+19,strlen(linebuf+19));
            SHA1((unsigned char *) &linebuf + 19, strlen(linebuf + 19), (unsigned char *) &sha1_data);
//            printf("sha1:%s\n",sha1_data);
            int l = strlen(reinterpret_cast<const char *>(sha1_data));
            base64_encode(reinterpret_cast<char *>(sha1_data), l, sec_accept);
//            printf("base64:%s\n",sec_accept);
            /* write the response */
            sprintf(head, "HTTP/1.1 101 Switching Protocols\r\n" \
                          "Upgrade: websocket\r\n" \
                          "Connection: Upgrade\r\n" \
                          "Sec-WebSocket-Accept: %s\r\n" \
                          "\r\n", sec_accept);

            printf("response\n");
            printf("%s", head);
            if (write(cli_fd, head, strlen(head)) < 0)
                perror("write");

            break;
        }
    } while ((buffer[level] != '\r' || buffer[level + 1] != '\n') && level != -1);
    return 0;
}

void inverted_string(char *str, int len) {
    int i;
    char temp;
    for (i = 0; i < len / 2; ++i) {
        temp = *(str + i);
        *(str + i) = *(str + len - i - 1);
        *(str + len - i - 1) = temp;
    }
}

int recv_frame_head(int fd, frame_head *head) {
    char one_char;
    /*read fin and op code*/
    if (read(fd, &one_char, 1) <= 0) {
        perror("read fin");
        return -1;
    }
    head->fin = (one_char & 0x80) == 0x80;
    head->opcode = one_char & 0x0F;
    if (read(fd, &one_char, 1) <= 0) {
        perror("read mask");
        return -1;
    }
    head->mask = (one_char & 0x80) == 0X80;

    /*get payload length*/
    head->payload_length = one_char & 0x7F;

    if (head->payload_length == 126) {
        char extern_len[2];
        if (read(fd, extern_len, 2) <= 0) {
            perror("read extern_len");
            return -1;
        }
        head->payload_length = (extern_len[0] & 0xFF) << 8 | (extern_len[1] & 0xFF);
    } else if (head->payload_length == 127) {
        char extern_len[8];
        if (read(fd, extern_len, 8) <= 0) {
            perror("read extern_len");
            return -1;
        }
        inverted_string(extern_len, 8);
        memcpy(&(head->payload_length), extern_len, 8);
    }

    /*read masking-key*/
    if (read(fd, head->masking_key, 4) <= 0) {
        perror("read masking-key");
        return -1;
    }

    return 0;
}

/**
 * @brief umask
 * xor decode
 * @param data 传过来时为密文，解码后的明文同样存储在这里
 * @param len data的长度
 * @param mask 掩码
 */
void umask(char *data, int len, char *mask) {
    int i;
    for (i = 0; i < len; ++i)
        *(data + i) ^= *(mask + (i % 4));
}

int send_frame_head(int fd, frame_head *head) {
    char *response_head;
    int head_length = 0;
    if (head->payload_length < 126) {
        response_head = (char *) malloc(2);
        response_head[0] = 0x81;
        response_head[1] = head->payload_length;
        head_length = 2;
    } else if (head->payload_length < 0xFFFF) {
        response_head = (char *) malloc(4);
        response_head[0] = 0x81;
        response_head[1] = 126;
        response_head[2] = (head->payload_length >> 8 & 0xFF);
        response_head[3] = (head->payload_length & 0xFF);
        head_length = 4;
    } else {
        response_head = (char *) malloc(12);
        response_head[0] = 0x81;
        response_head[1] = 127;
        memcpy(response_head + 2, reinterpret_cast<const void *>(head->payload_length), sizeof(unsigned long long));
        inverted_string(response_head + 2, sizeof(unsigned long long));
        head_length = 12;
    }

    if (write(fd, response_head, head_length) <= 0) {
        perror("write head");
        return -1;
    }

    free(response_head);
    return 0;
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

                event.events = EPOLLIN;        //表示对应的文件描述符可读（包括对端SOCKET正常关闭）
                event.data.fd = connFd;//将connFd设置为要读取的文件描述符
                //event.data.ptr = &head;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connFd, &event) == -1) {
                    perror("epoll_ctl:conn_fd register failed");
                    exit(EXIT_FAILURE);
                }
            } else if (events[i].events & EPOLLIN) {
                printf("recieve\n");
                frame_head head;
                int rul = recv_frame_head(events[i].data.fd, &head);
                if (rul < 0) {
                    close(events[i].data.fd);
                    break;
                }
                send_frame_head(events[i].data.fd, &head);
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
                    if (write(events[i].data.fd, payload_data, rul) <= 0)
                        break;
                } while (size < head.payload_length);
                if (head.opcode == 0x8) {
                    printf("socket %d close\n", events[i].data.fd, payload_data, 1024);
                    mapPlayer.erase(events[i].data.fd);
                    close(events[i].data.fd);
                }
                printf("first:%d\n", events[i].data.fd);
                processRequest(payload_data, events[i], epoll_fd, i);
                printf("\n-----------\n");

            } else {
                printf("write\n");
                cJSON *response = cJSON_CreateObject();

                if (mapPlayer[events[i].data.fd].event == 1) {
                    cJSON_AddNumberToObject(response, "function", 1);
                    cJSON_AddNumberToObject(response, "type", 2);
                    cJSON *data = cJSON_CreateArray();
                    for (map<int, room>::iterator iter = mapRoom.begin(); iter != mapRoom.end(); iter++){
                        cJSON *r = cJSON_CreateObject();
                        cJSON_AddNumberToObject(r,"id",iter->first);
                        cJSON_AddNumberToObject(r,"amount",iter->second.getConnectAmount());
                        cJSON_AddStringToObject(r,"name",iter->second.getName());

                        cJSON_AddItemToArray(data, r);
                    }
                    cJSON_AddItemToObject(response,"data",data);
                    char *json = cJSON_PrintUnformatted(response);
                    frame_head head;
                    head.payload_length = strlen(json);
                    send_frame_head(events[i].data.fd, &head);
                    write(events[i].data.fd, json, strlen(json));

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

