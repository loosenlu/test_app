//
// Created by loosen on 16/1/4.
//

#include "header.h"



/*
 *  Initialize a array of sockets, every socket of the array
 *  is connected;
 *
 *  @param connect_arry:  the pointer to array;
 *  @param num: the number of array, is a value-result param,
 *            return number of connected socket;
 *  @param servaddr: address of server;
 *  @param servaddr_len: length of address
 *  @return: the max-socketfd for select function
 */
int connections_init(struct connect_info *usr_arr,
                    struct sockaddr *servaddr, socklen_t servaddr_len)
{
    int sockets_tmp[usr_arr->valid_num];
    int effect_sockets_num, counter, maxfd, i;

    // create sockets for connect
    for (counter = 0; counter < (usr_arr->valid_num); ++counter) {

        if ((sockets_tmp[counter] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            // printf("The %dth socket is failure.\n", counter);
            continue;
        }
    }

    // connect to server
    effect_sockets_num = 0;
    for (counter = 0; counter < usr_arr->valid_num; ++counter) {

        if (sockets_tmp[counter] > 0) {
            if (connect(sockets_tmp[counter], servaddr, servaddr_len) == 0)
                ++effect_sockets_num;
            else
            {
                sockets_tmp[counter] = -1;
                printf("connect is failure: %s", strerror(errno));
                //fflush(stdout);
            }
        }
    }

    maxfd = 0;
    for (i = 0, counter = 0; counter < usr_arr->valid_num; ++counter) {
        if (sockets_tmp[counter] > 0) {

            usr_arr->arr[i].socketfd = sockets_tmp[counter];
            usr_arr->arr[i].usrID = (uint16_t)i;
            usr_arr->arr[i].error = 0;
            usr_arr->arr[i].flags = FVALID;
            usr_arr->arr[i].egg_map = 0;
            memset(usr_arr->arr[i].recvbuf, 0, MAXLENRECVBUF);
            if (maxfd < sockets_tmp[counter])
                maxfd = sockets_tmp[counter];
            ++i;
        }
    }

    usr_arr->valid_num = effect_sockets_num;
    usr_arr->maxfd = maxfd;
    return 0;
}


/*
 *  Build the packet for communication with server
 *  @param buf: the packet buffer
 *  @param buflen: the buffer length return by this function
 *  @param type: packet type
 *  @return: 0 means OK
 */
int packet_builder(char **buf, int *buflen, int type)
{
    switch (type) {
        case SGAMEKNOCK:
        { // 砸蛋数据包
            *buf = (char *)malloc(LEGGKNOCK * sizeof(char));
            (*buf)[0] = 0x0a;
            (*buf)[1] = SGAMEKNOCK;
            (*buf)[2] = 0x05;
            (*buf)[LEGGKNOCK - 1] = 0x55;
            *buflen = LEGGKNOCK;
            return 0;
        }

        case SGAMEENTER:
        { // 选择场次
            *buf = (char *)malloc(LENTERGAME * sizeof(char));
            (*buf)[0] = 0x0a;
            (*buf)[1] = SGAMEENTER;
            (*buf)[2] = 0x04;
            (*buf)[5] = 0x01; // 场景1
            (*buf)[6] = 0x01; // VIP
            (*buf)[LENTERGAME - 1] = 0x55;
            *buflen = LENTERGAME;
            return 0;
        }
        default:
        { // default
            return 1;
        }
    }
}




void enter_game(struct connect_info *usr_info)
{
    char *sendbuf;
    int buflen, i;
    fd_set rset;

    packet_builder(&sendbuf, &buflen, SGAMEENTER);

    for(i = 0; i < usr_info->valid_num; ++i) {
        memcpy(sendbuf + 3, &usr_info->arr[i].usrID, 2);
        if (write(usr_info->arr[i].socketfd, sendbuf, LENTERGAME) != LENTERGAME)
        {
            usr_info->arr[i].error = EGAMEENTER;
            usr_info->arr[i].flags = FINVALID;
        }
    }

    FD_ZERO(&rset);
    for(i = 0; i < usr_info->valid_num; ++i) {
        if(usr_info->arr[i].flags == FVALID)
        {
            FD_SET(usr_info->arr[i].socketfd, &rset);
        }
    }

    select(usr_info->maxfd, &rset, NULL, NULL, NULL);
    for(i = 0; i < usr_info->valid_num; ++i) {
        if (FD_ISSET(usr_info->arr[i].socketfd, &rset)) {
            read(usr_info->arr[i].socketfd, usr_info->arr[i].recvbuf, 22);
        }
    }
}


int packet_process(struct connect_info *usr_info, fd_set set)
{
    int i;
    ssize_t  length;
    for(i = 0; i < usr_info->valid_num; ++i) {
        if (FD_ISSET(usr_info->arr[i].socketfd, &set))
        {
            length = recv(usr_info->arr[i].socketfd,
                          usr_info->arr[i].recvbuf,MAXLENRECVBUF, 0);
            switch(usr_info->arr[i].recvbuf[1])
            {
                case SGAMEEGGSINFO:
                {
                    if (length == LEGGINFO)
                    {
                        memcpy(&usr_info->arr[i].egg_map,
                               usr_info->arr[i].recvbuf + 5, 4);
                        usr_info->arr[i].flags = FVALID;
                    }
                    else
                    {
                        usr_info->arr[i].flags = FMAPOUTOFTIME;
                    }
                }

                case SGAMEKNOCKRET:
                {
                    if (length == LKNOCKRET)
                        ;
                }
                default:
                {
                    return 1;
                }
            }
        }
    }
}