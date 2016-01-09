//
// Created by loosen on 16/1/4.
//

#ifndef TEST_APP_HEADER_H
#define TEST_APP_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <time.h>



#define SGAMEENTER 0x27
#define SGAMEKNOCK 0x31

#define SGAMEEGGSINFO 0x30
#define SGAMEKNOCKRET 0x32

#define MAXLENRECVBUF 34
#define LENTERGAME 8
#define LEGGKNOCK 9
#define LEGGINFO 9
#define LKNOCKRET 34

#define EGAMEENTER 0x01

#define FINVALID 0x02
#define FVALID 0x01
#define FMAPOUTOFTIME 0x04

// structure
struct connect_item {
    int socketfd;
    uint16_t usrID;
    uint8_t flags;
    uint8_t error;
    uint32_t egg_map;
    char recvbuf[MAXLENRECVBUF];
};


struct connect_info {
    struct connect_item *arr;
    int valid_num;
    int maxfd;
};



// utility function
int connections_init(struct connect_info *, struct sockaddr *, socklen_t);
int packet_builder(char **buf, int *buflen, int type);
int packet_process(struct connect_info *, fd_set set);


void enter_game(struct connect_info *);

#endif //TEST_APP_HEADER_H
