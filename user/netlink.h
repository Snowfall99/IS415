#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/netlink.h>
#include <sys/socket.h>


#define NETLINK_TEST 30
#define MSG_LEN 128
#define MAX_PAYLOAD 128

struct user_msg_info {
    struct nlmsghdr hdr;
    char msg[MSG_LEN];
};

int my_send_msg(char*);
int my_recv_msg(void);

int my_send_msg(char* umsg_) {
    int sk_fd;
    int ret;
    struct user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl src_addr, dst_addr;
    
    sk_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(sk_fd == -1) {
        perror("create socket error\n");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = 200;
    src_addr.nl_groups = 0;
    if(bind(sk_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) != 0) {
        perror("bind() error\n");
        close(sk_fd);
        return -1;
    }

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0;
    dst_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = src_addr.nl_pid;

    memcpy(NLMSG_DATA(nlh), umsg_, strlen(umsg_));
    ret = sendto(sk_fd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dst_addr, sizeof(struct sockaddr_nl));
    if(!ret) {
        perror("sendto error\n");
        close(sk_fd);
        exit(-1);
    }
    close(sk_fd);
    free((void *)nlh);
    return ret;
}

int my_recv_msg(char* umsg_) {
    int sk_fd;
    int ret;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl src_addr, dst_addr;

    sk_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(sk_fd == -1) {
        perror("create socket error\n");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = 100;
    src_addr.nl_groups = 0;
    if(bind(sk_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) != 0) {
        perror("bind() error\n");
        close(sk_fd);
        return -1;
    }

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0;
    dst_addr.nl_groups = 0;

    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    ret = recvfrom(sk_fd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&src_addr, &len);
    if(!ret) {
        perror("recv form kernel error\n");
        close(sk_fd);
        exit(-1);
    }
    
    umsg_ = u_info.msg;
    close(sk_fd);
    free((void *)nlh);
    return 0;
}