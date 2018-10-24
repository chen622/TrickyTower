//
// Created by chen on 2018/10/22.
//

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include "room.h"
//#include "semaphore.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>
#include <sys/shm.h>
#include <sys/ipc.h>
/*在握手时需要进行sha1编码和base64编码，
在这里用openssl的库来实现*/

#define MAX_EVENTS 20
#define BUFFER_SIZE 1024
#define RESPONSE_HEADER_LEN_MAX 1024
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"


/*-----------为了便于理解，在这里吧数据帧格式粘出来-------------------
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
--------------------------------------------------------------------*/
typedef struct _frame_head {
    char fin;
    char opcode;
    char mask;
    unsigned long long payload_length;
    char masking_key[4];
} frame_head;

struct player {
    int room_id = -1;
    int event = -1;//1表示房间请求
};

#endif //WEBSOCKET_SERVER_H
