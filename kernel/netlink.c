#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <net/sock.h>

#include "privilege.h"

#define NETLINK_TEST 30
#define USER_PORT 100

struct sock *nlsk = NULL;
extern struct net init_net;

unsigned int msg_array[1024];

int send_msg(char* pbuf, uint16_t len) {
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;

    int ret;

    // alloc space for sk_buff
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if (!nl_skb) {
        printk(KERN_INFO "netlink alloc fail\n");
        return -1;
    }

    // set netlink header
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
    if (nlh == NULL) {
        printk(KERN_INFO "nlmsg_put fail\n");
        nlmsg_free(nl_skb);
        return -1;
    }

    // copy data for sending 
    memcpy(nlmsg_data(nlh), pbuf, len);
    ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);

    return ret;

}

static void netlink_rcv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = NULL;
    char* umsg = NULL;
    int i, j;
    int len;
    int temp_len;
    if (skb->len >= nlmsg_total_size(0)) {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if (umsg) {
            printk(KERN_INFO "kernel receive from user: %s\n", umsg);
            char* command_;
            char* exe_file_;
            exe_file_ = kmalloc(128, GFP_KERNEL);
            char* kind_;
            kind_ = kmalloc(32, GFP_KERNEL);
            int write_;
            int read_;

            // TODO
            // decode msg from user
            if (strstr(umsg, "add") != NULL) {
                command_ = "add";
                len = strlen(umsg);
                for (i = 4; i < len && umsg[i] != ' '; i++) {
                    temp_len = strlen(exe_file_);
                    exe_file_[temp_len] = umsg[i];
                    exe_file_[temp_len+1] = '\0';
                }
                i += 1;
                for (; i < len && umsg[i] != ' '; i++) {
                    temp_len = strlen(kind_);
                    kind_[temp_len] = umsg[i];
                    kind_[temp_len+1] = '\0';
                }
                i += 1;
                write_ = (umsg[i] - '0');
                i += 2;
                read_ = (umsg[i] - '0');
                printk(KERN_INFO "add:%s,%s,%d,%d\n", exe_file_, kind_, write_, read_);
            } else if (strstr(umsg, "modify") != NULL) {
                command_ = "modify";
                len = strlen(umsg);
                for (i = 7; i < len && umsg[i] != ' '; i++) {
                    temp_len = strlen(exe_file_);
                    exe_file_[temp_len] = umsg[i];
                    exe_file_[temp_len+1] = '\0';
                }
                i += 1;
                for (; i < len && umsg[i] != ' '; i++) {
                    temp_len = strlen(kind_);
                    kind_[temp_len] = umsg[i];
                    kind_[temp_len+1] = '\0';
                }
                i += 1;
                write_ = (umsg[i] - '0');
                i += 2;
                read_ = (umsg[i] - '0');
                printk(KERN_INFO "modify:%s,%s,%d,%d\n", exe_file_, kind_, write_, read_);
            } else if (strstr(umsg, "delete") != NULL) {
                printk(KERN_INFO "delete\n");
                command_ = "delete";
                exe_file_ = "test";
                kind_ = "txt";
                write_ = 0;
                read_ = 0;
            }
            if (strcmp(command_, "add") == 0) {
                add_privilege(exe_file_, write_, read_, kind_);
            } else if (strcmp(command_, "modify") == 0) {
                modify_privilege(exe_file_, write_, read_, kind_, 0);
            } else if (strcmp(command_, "delete") == 0) {
                delete_privilege(exe_file_, write_, read_, kind_, 0);
            }
        }
    }
}

struct netlink_kernel_cfg cfg = {
    .input = netlink_rcv_msg,
};

int init_netlink(void) {
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!nlsk) {
        printk(KERN_INFO, "netlink kernel create error\n");
        return -1;
    }
    // init message array
    int i;
    for (i = 0; i < 1024; i++) {
        msg_array[i] = 0;
    }
    printk(KERN_INFO "netlink init\n");
    return 0;
}

void remove_netlink(void) {
    if (nlsk) {
        netlink_kernel_release(nlsk);
        nlsk = NULL;
    }
    printk(KERN_INFO "netlink release\n");
}