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

    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if (!nl_skb) {
        printk(KERN_INFO "netlink alloc fail\n");
        return -1;
    }

    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
    if (nlh == NULL) {
        printk(KERN_INFO "nlmsg_put fail\n");
        nlmsg_free(nl_skb);
        return -1;
    }

    memcpy(nlmsg_data(nlh), pbuf, len);
    ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);

    return ret;

}

static void my_rcv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = NULL;
    char* umsg = NULL;
    int i;
    int len;
    int temp_len;

    char* command_;
    char* exe_;
    char* type_;
    int write_;
    int read_;

    if (skb->len >= nlmsg_total_size(0)) {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if (umsg) {
            printk(KERN_INFO "kernel receive from user: %s\n", umsg);
            command_ = kmalloc(16, GFP_KERNEL);
            exe_ = kmalloc(128, GFP_KERNEL);
            type_ = kmalloc(32, GFP_KERNEL);
            len = strlen(umsg);

            i = 0;
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(command_);
                command_[temp_len] = umsg[i];
                command_[temp_len+1] = '\0';
                i ++;
            }

            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(exe_);
                exe_[temp_len] = umsg[i];
                exe_[temp_len+1] = '\0';
                i ++;
            }

            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(type_);
                type_[temp_len] = umsg[i];
                type_[temp_len+1] = '\0';
                i ++;
            }
            
            while (umsg[i] == ' ' && i < len) {
                i++;
            }
            write_ = (umsg[i] - '0');
            i ++;
            
            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            read_ = (umsg[i] - '0');

            printk(KERN_INFO "%s:%s,%s,%d,%d\n", command_, exe_, type_, write_, read_);

            if (strcmp(command_, "add") == 0) {
                add_privilege(exe_, type_, write_, read_);
            } else if (strcmp(command_, "update") == 0) {
                update_privilege(exe_, type_, write_, read_);
            } else if (strcmp(command_, "delete") == 0) {
                delete_privilege(exe_, type_);
            } else if (strcmp(command_, "list") == 0) {
                list_privilege();
            }
        }
    }
}

struct netlink_kernel_cfg cfg = {
    .input = my_rcv_msg,
};

int init_netlink(void) {
    int i;

    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!nlsk) {
        printk(KERN_INFO "netlink kernel create error\n");
        return -1;
    }

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