//
// Created by chen on 2018/10/24.
//

#ifndef TRICKYTOWERSERVER_WEBSOCKET_H
#define TRICKYTOWERSERVER_WEBSOCKET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cjson/cJSON.h>
#include <sys/sem.h>

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

int send_frame_head(int fd, frame_head *head);
int passive_server(int port, int queue);
int base64_encode(char *in_str, int in_len, char *out_str);
int _readline(char *allbuf, int level, char *linebuf);
int shakehands(int cli_fd);
void inverted_string(char *str, int len);
int recv_frame_head(int fd, frame_head *head);
void umask(char *data, int len, char *mask);
int send_frame_head(int fd, frame_head *head);
int send_msg(int fd,char *str);


#endif //TRICKYTOWERSERVER_WEBSOCKET_H
