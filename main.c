
#include "header.h"

int
main(int argc, char **argv)
{

    char *Cservaddr = "192.168.206.1";
    fd_set rset, wset;
    int i;

    struct connect_info usr_info;
    usr_info.arr = NULL;
    usr_info.valid_num = 10;
    usr_info.maxfd = 0;

    struct connect_item connects[usr_info.valid_num];
    usr_info.arr = connects;

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8881);
    if (inet_pton(AF_INET, Cservaddr, &servaddr.sin_addr.s_addr) <= 0)
    {
        printf("inet_pton is failure\n");
        fflush(stdout);
    }


    if (connections_init(&usr_info, (struct sockaddr *)&servaddr,
                         sizeof(servaddr)) == 0) {
        printf("connection init success!\n");
        fflush(stdout);
    }

    enter_game(&usr_info);

    FD_ZERO(&rset);
    FD_ZERO(&wset);
    for(i = 0; i < usr_info.valid_num; ++i) {
        FD_SET(usr_info.arr[i].socketfd, &rset);
    }
    if (select(usr_info.maxfd, &rset, NULL, NULL, NULL) > 0) {
        packet_process(&usr_info, rset);
    }

    for(i = 0; i < usr_info.valid_num; ++i) {

    }

    return 0;
}