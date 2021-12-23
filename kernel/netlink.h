/*
 * Netlink module in kernel mode.
 */
#ifndef __KERNEL_NETLINK_H__
#define __KERNEL_NETLINK_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <net/sock.h>

#include "privilege.h"

#define NETLINK_TEST 30
#define USER_PORT 100

struct sock *nlsk = NULL;
extern struct net init_net;

unsigned int msg_array[1024];

void addPrivilege(char*, char*, char*, int);
void updatePrivilege(char*, char*, char*, int);
void deletePrivilege(char*, char*, char*);
void listPrivilege(void);

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

static void recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = NULL;
    char* umsg = NULL;
    int i;
    int len;
    int temp_len;
    char* command_;
    char* privilege_;
    char* exe_;
    char* type_;
    int value_;

    if (skb->len >= nlmsg_total_size(0)) {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if (umsg) {
            printk(KERN_INFO "kernel receive from user: %s\n", umsg);
            // Malloc space to hold message from netlink.
            command_ = kmalloc(16, GFP_KERNEL);
            exe_ = kmalloc(128, GFP_KERNEL);
            type_ = kmalloc(32, GFP_KERNEL);
            privilege_ = kmalloc(32, GFP_KERNEL);
            len = strlen(umsg);
            i = 0;
            // Get command.
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(command_);
                command_[temp_len] = umsg[i];
                command_[temp_len+1] = '\0';
                i ++;
            }
            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            // Get privilege name.
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(privilege_);
                privilege_[temp_len] = umsg[i];
                privilege_[temp_len+1] = '\0';
                i ++;
            }
            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            // Get executable's name.
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(exe_);
                exe_[temp_len] = umsg[i];
                exe_[temp_len+1] = '\0';
                i ++;
            }
            while (umsg[i] == ' ' && i < len) {
                i ++;
            }
            // Get type. 
            while (umsg[i] != ' ' && i < len) {
                temp_len = strlen(type_);
                type_[temp_len] = umsg[i];
                type_[temp_len+1] = '\0';
                i ++;
            }
            while (umsg[i] == ' ' && i < len) {
                i++;
            }
            // Get value.
            value_ = (umsg[i] - '0');
            
            printk(KERN_INFO "%s:%s,%s,%s,%d\n", command_, privilege_, exe_, type_, value_);
            if (strcmp(command_, "add") == 0) {
                addPrivilege(privilege_, exe_, type_, value_);
            } else if (strcmp(command_, "update") == 0) {
                updatePrivilege(privilege_, exe_, type_, value_);
            } else if (strcmp(command_, "delete") == 0) {
                deletePrivilege(privilege_, exe_, type_);
            } else if (strcmp(command_, "list") == 0) {
                listPrivilege();
            }
        }
    }
}

void list(char* privilegeName_, struct Privilege privileges_[]) {
    // Used for message sending.
    char msg[64];
    int i;
    
    sprintf(msg, "\n\033[035m%s\033[0m\n", privilegeName_);
    send_msg(msg, strlen(msg));
    mdelay(10);
    sprintf(msg, "\033[033m%-16s %-16s %s\033[0m", "<executable>", "<target>", "<val>");
    send_msg(msg, strlen(msg));
    mdelay(10);
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (privileges_[i].tombstone) {
            sprintf(msg, "\033[32m%-16s\033[0m \033[34m%-16s\033[0m %d", 
                privileges_[i].exe, privileges_[i].target, 
                privileges_[i].value);
            send_msg(msg, strlen(msg));
            mdelay(10);
        }
    }
    return;
}

void listPrivilege(void) {
    list("write", WritePrivilege);
    list("read", ReadPrivilege);
    list("open", OpenPrivilege);
    list("chmod", ChmodPrivilege);
    list("creat", CreatPrivilege);
    list("mkdir", MkdirPrivilege);
    list("rmdir", RmdirPrivilege);
    return;
}

void add(char* exe_, char* target_, int value_, struct Privilege privileges_[]) {
    // Record the position for insert.
    int pos = 0;
    // Whether there is a duplicated privilege or not.
    int flag = 1;
    // Reponse error message to user mode.
    char resp[64];
    int i;

    // Check whether there is a duplicated privilege or not.
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(privileges_[i].exe, exe_) == 0 && 
            strcmp(privileges_[i].target, target_) == 0 && 
            privileges_[i].tombstone) {
            flag = 0;
            break;
        }
        if (!privileges_[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    // response error msg to user mode
    if (!flag) {
        strcpy(resp, "\033[31merror:\033[0m there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(privileges_[pos].exe, exe_);
    strcpy(privileges_[pos].target, target_);
    privileges_[pos].tombstone = 1;
    privileges_[pos].value = value_;
    return;
}

void addPrivilege(char* privilege_, char* exe_, char* target_, int value_) {
    if (strcmp(privilege_, "write") == 0) {
        add(exe_, target_, value_, WritePrivilege);
    } else if (strcmp(privilege_, "read") == 0) {
        add(exe_, target_, value_, ReadPrivilege);
    } else if (strcmp(privilege_, "open") == 0) {
        add(exe_, target_, value_, OpenPrivilege);
    } else if (strcmp(privilege_, "creat") == 0) {
        add(exe_, target_, value_, CreatPrivilege);
    } else if (strcmp(privilege_, "chmod") == 0) {
        add(exe_, target_, value_, ChmodPrivilege);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        add(exe_, target_, value_, MkdirPrivilege);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        add(exe_, target_, value_, RmdirPrivilege);
    }
    return;
}

void update(char* exe_, char* target_, int value_, struct Privilege privileges_[]) {
    // response error message to user mode
    char resp[128];
    int i;

    // Look for the privilege and update.
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(privileges_[i].exe, exe_) == 0 && 
            strcmp(privileges_[i].target, target_) == 0 && 
            privileges_[i].tombstone) {
            privileges_[i].value = value_;
            break;
        }
    }
    // If not found, return error message to user mode.
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "\033[31merror:\033[0m privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void updatePrivilege(char* privilege_, char* exe_, char* target_, int value_) {
    if (strcmp(privilege_, "write") == 0) {
        update(exe_, target_, value_, WritePrivilege);
    } else if (strcmp(privilege_, "read") == 0) {
        update(exe_, target_, value_, ReadPrivilege);
    } else if (strcmp(privilege_, "open") == 0) {
        update(exe_, target_, value_, OpenPrivilege);
    } else if (strcmp(privilege_, "creat") == 0) {
        update(exe_, target_, value_, CreatPrivilege);
    } else if (strcmp(privilege_, "chmod") == 0) {
        update(exe_, target_, value_, ChmodPrivilege);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        update(exe_, target_, value_, MkdirPrivilege);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        update(exe_, target_, value_, RmdirPrivilege);
    }
    return;
}

void del(char* exe_, char* target_, struct Privilege privileges_[]) {
    // Response error message to user mode.
    char resp[128];
    int i;

    // Look for privilege and change its tombstone to 0.
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(privileges_[i].exe, exe_) == 0 && 
            strcmp(privileges_[i].target, target_) == 0 && 
            privileges_[i].tombstone) {
            privileges_[i].tombstone = 0;
            break;
        }
    }
    // If not found, return error message to user mode.
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "\033[031merror:\033[0m privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void deletePrivilege(char* privilege_, char* exe_, char* target_) {
    if (strcmp(privilege_, "write") == 0) {
        del(exe_, target_, WritePrivilege);
    } else if (strcmp(privilege_, "read") == 0) {
        del(exe_, target_, ReadPrivilege);
    } else if (strcmp(privilege_, "open") == 0) {
        del(exe_, target_, OpenPrivilege);
    } else if (strcmp(privilege_, "creat") == 0) {
        del(exe_, target_, CreatPrivilege);
    } else if (strcmp(privilege_, "chmod") == 0) {
        del(exe_, target_, ChmodPrivilege);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        del(exe_, target_, MkdirPrivilege);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        del(exe_, target_, RmdirPrivilege);
    }
    return;
}

struct netlink_kernel_cfg cfg = {
    // Hook recv_msg for receiving msg from netlink.
    .input = recv_msg,
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

#endif // __KERNEL_NETLINK_H__
