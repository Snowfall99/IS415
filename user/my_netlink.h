#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/netlink.h>
#include <sys/socket.h>


#define NETLINK_TEST    30
#define MSG_LEN         128
#define MAX_PLOAD       128

typedef struct _user_msg_info {
    struct nlmsghdr hdr;
    char msg[MSG_LEN];
} user_msg_info;

int mysendmsg(char* umsg);
int myrecvmsg(void);