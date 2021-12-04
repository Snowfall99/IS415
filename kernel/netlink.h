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

void add_privilege(char*, char*, char*, int);
void update_privilege(char*, char*, char*, int);
void delete_privilege(char*, char*, char*);
void list_privilege(void);

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
    char* privilege_;
    char* exe_;
    char* type_;
    int value_;

    if (skb->len >= nlmsg_total_size(0)) {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if (umsg) {
            printk(KERN_INFO "kernel receive from user: %s\n", umsg);
            command_ = kmalloc(16, GFP_KERNEL);
            exe_ = kmalloc(128, GFP_KERNEL);
            type_ = kmalloc(32, GFP_KERNEL);
            privilege_ = kmalloc(32, GFP_KERNEL);
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
                temp_len = strlen(privilege_);
                privilege_[temp_len] = umsg[i];
                privilege_[temp_len+1] = '\0';
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
            value_ = (umsg[i] - '0');
            
            printk(KERN_INFO "%s:%s,%s,%s,%d\n", command_, privilege_, exe_, type_, value_);

            if (strcmp(command_, "add") == 0) {
                add_privilege(privilege_, exe_, type_, value_);
            } else if (strcmp(command_, "update") == 0) {
                update_privilege(privilege_, exe_, type_, value_);
            } else if (strcmp(command_, "delete") == 0) {
                delete_privilege(privilege_, exe_, type_);
            } else if (strcmp(command_, "list") == 0) {
                list_privilege();
            }
        }
    }
}

void list_privilege(void) {
    char msg[64];
    int i;

    sprintf(msg, "%-16s\n", "write");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (WritePrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", WritePrivilege[i].exe, WritePrivilege[i].target, WritePrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "read");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (ReadPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", ReadPrivilege[i].exe, ReadPrivilege[i].target, ReadPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "open");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (OpenPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", OpenPrivilege[i].exe, OpenPrivilege[i].target, OpenPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "mkdir");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (MkdirPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", MkdirPrivilege[i].exe, MkdirPrivilege[i].target, MkdirPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "rmdir");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (RmdirPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", RmdirPrivilege[i].exe, RmdirPrivilege[i].target, RmdirPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "chmod");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (ChmodPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", ChmodPrivilege[i].exe, ChmodPrivilege[i].target, ChmodPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    sprintf(msg, "%-16s\n", "creat");
    send_msg(msg, strlen(msg));
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (CreatPrivilege[i].tombstone) {
            sprintf(msg, "%-16s %-16s %d", CreatPrivilege[i].exe, CreatPrivilege[i].target, CreatPrivilege[i].value);
            send_msg(msg, strlen(msg));
        }
    }
    return;
}

void add_write(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(WritePrivilege[i].exe, exe_) == 0 && strcmp(WritePrivilege[i].target, target_) == 0 && WritePrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!WritePrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(WritePrivilege[pos].exe, exe_);
    strcpy(WritePrivilege[pos].target, target_);
    WritePrivilege[pos].tombstone = 1;
    WritePrivilege[pos].value = value_;
    return;
}

void add_read(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ReadPrivilege[i].exe, exe_) == 0 && strcmp(ReadPrivilege[i].target, target_) == 0 && ReadPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!ReadPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(ReadPrivilege[pos].exe, exe_);
    strcpy(ReadPrivilege[pos].target, target_);
    ReadPrivilege[pos].tombstone = 1;
    ReadPrivilege[pos].value = value_;
    return;
}

void add_open(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(OpenPrivilege[i].exe, exe_) == 0 && strcmp(OpenPrivilege[i].target, target_) == 0 && OpenPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!OpenPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(OpenPrivilege[pos].exe, exe_);
    strcpy(OpenPrivilege[pos].target, target_);
    OpenPrivilege[pos].tombstone = 1;
    OpenPrivilege[pos].value = value_;
    return;
}

void add_chmod(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ChmodPrivilege[i].exe, exe_) == 0 && strcmp(ChmodPrivilege[i].target, target_) == 0 && ChmodPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!ChmodPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(ChmodPrivilege[pos].exe, exe_);
    strcpy(ChmodPrivilege[pos].target, target_);
    ChmodPrivilege[pos].tombstone = 1;
    ChmodPrivilege[pos].value = value_;
    return;
}

void add_creat(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(CreatPrivilege[i].exe, exe_) == 0 && strcmp(CreatPrivilege[i].target, target_) == 0 && CreatPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!CreatPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(CreatPrivilege[pos].exe, exe_);
    strcpy(CreatPrivilege[pos].target, target_);
    CreatPrivilege[pos].tombstone = 1;
    CreatPrivilege[pos].value = value_;
    return;
}

void add_mkdir(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(MkdirPrivilege[i].exe, exe_) == 0 && strcmp(MkdirPrivilege[i].target, target_) == 0 && MkdirPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!MkdirPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(MkdirPrivilege[pos].exe, exe_);
    strcpy(MkdirPrivilege[pos].target, target_);
    MkdirPrivilege[pos].tombstone = 1;
    MkdirPrivilege[pos].value = value_;
    return;
}

void add_rmdir(char* exe_, char* target_, int value_) {
    // record the position for insert
    int pos = 0;
    int i;
    int flag = 1;
    char resp[64];
    // check whether there is a duplicated privilege or not 
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(RmdirPrivilege[i].exe, exe_) == 0 && strcmp(RmdirPrivilege[i].target, target_) == 0 && RmdirPrivilege[i].tombstone) {
            flag = 0;
            break;
        }
        if (!RmdirPrivilege[i].tombstone && pos == 0) {
            pos = i;
        }
    }
    if (!flag) {
        strcpy(resp, "there is a duplicated privilege existing");
        send_msg(resp, strlen(resp));
        return;
    }
    // insert the privilege into list
    strcpy(RmdirPrivilege[pos].exe, exe_);
    strcpy(RmdirPrivilege[pos].target, target_);
    RmdirPrivilege[pos].tombstone = 1;
    RmdirPrivilege[pos].value = value_;
    return;
}


void add_privilege(char* privilege_, char* exe_, char* target_, int value_) {
    if (strcmp(privilege_, "write") == 0) {
        add_write(exe_, target_, value_);
    } else if (strcmp(privilege_, "read") == 0) {
        add_read(exe_, target_, value_);
    } else if (strcmp(privilege_, "open") == 0) {
        add_open(exe_, target_, value_);
    } else if (strcmp(privilege_, "creat") == 0) {
        add_creat(exe_, target_, value_);
    } else if (strcmp(privilege_, "chmod") == 0) {
        add_chmod(exe_, target_, value_);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        add_mkdir(exe_, target_, value_);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        add_rmdir(exe_, target_, value_);
    }
    return;
}

void update_write(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(WritePrivilege[i].exe, exe_) == 0 && strcmp(WritePrivilege[i].target, target_) == 0 && WritePrivilege[i].tombstone) {
            WritePrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_read(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ReadPrivilege[i].exe, exe_) == 0 && strcmp(ReadPrivilege[i].target, target_) == 0 && ReadPrivilege[i].tombstone) {
            ReadPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_open(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(OpenPrivilege[i].exe, exe_) == 0 && strcmp(OpenPrivilege[i].target, target_) == 0 && OpenPrivilege[i].tombstone) {
            OpenPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_chmod(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ChmodPrivilege[i].exe, exe_) == 0 && strcmp(ChmodPrivilege[i].target, target_) == 0 && ChmodPrivilege[i].tombstone) {
            ChmodPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_creat(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(CreatPrivilege[i].exe, exe_) == 0 && strcmp(CreatPrivilege[i].target, target_) == 0 && CreatPrivilege[i].tombstone) {
            CreatPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_mkdir(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(MkdirPrivilege[i].exe, exe_) == 0 && strcmp(MkdirPrivilege[i].target, target_) == 0 && MkdirPrivilege[i].tombstone) {
            MkdirPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_rmdir(char* exe_, char* target_, int value_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(RmdirPrivilege[i].exe, exe_) == 0 && strcmp(RmdirPrivilege[i].target, target_) == 0 && RmdirPrivilege[i].tombstone) {
            RmdirPrivilege[i].value = value_;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void update_privilege(char* privilege_, char* exe_, char* target_, int value_) {
    if (strcmp(privilege_, "write") == 0) {
        update_write(exe_, target_, value_);
    } else if (strcmp(privilege_, "read") == 0) {
        update_read(exe_, target_, value_);
    } else if (strcmp(privilege_, "open") == 0) {
        update_open(exe_, target_, value_);
    } else if (strcmp(privilege_, "creat") == 0) {
        update_creat(exe_, target_, value_);
    } else if (strcmp(privilege_, "chmod") == 0) {
        update_chmod(exe_, target_, value_);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        update_mkdir(exe_, target_, value_);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        update_rmdir(exe_, target_, value_);
    }
    return;
}

void delete_write(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(WritePrivilege[i].exe, exe_) == 0 && strcmp(WritePrivilege[i].target, target_) == 0 && WritePrivilege[i].tombstone) {
            WritePrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_read(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ReadPrivilege[i].exe, exe_) == 0 && strcmp(ReadPrivilege[i].target, target_) == 0 && ReadPrivilege[i].tombstone) {
            ReadPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_open(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(OpenPrivilege[i].exe, exe_) == 0 && strcmp(OpenPrivilege[i].target, target_) == 0 && OpenPrivilege[i].tombstone) {
            OpenPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_creat(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(CreatPrivilege[i].exe, exe_) == 0 && strcmp(CreatPrivilege[i].target, target_) == 0 && CreatPrivilege[i].tombstone) {
            CreatPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_chmod(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(ChmodPrivilege[i].exe, exe_) == 0 && strcmp(ChmodPrivilege[i].target, target_) == 0 && ChmodPrivilege[i].tombstone) {
            ChmodPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_mkdir(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(MkdirPrivilege[i].exe, exe_) == 0 && strcmp(MkdirPrivilege[i].target, target_) == 0 && MkdirPrivilege[i].tombstone) {
            MkdirPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_rmdir(char* exe_, char* target_) {
    int i;
    char resp[128];
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(RmdirPrivilege[i].exe, exe_) == 0 && strcmp(RmdirPrivilege[i].target, target_) == 0 && RmdirPrivilege[i].tombstone) {
            RmdirPrivilege[i].tombstone = 0;
            break;
        }
    }
    if (i == MAX_PRIVILEGE_NUM) {
        strcpy(resp, "privilege not found");
        send_msg(resp, strlen(resp));
    }
    return;
}

void delete_privilege(char* privilege_, char* exe_, char* target_) {
    if (strcmp(privilege_, "write") == 0) {
        delete_write(exe_, target_);
    } else if (strcmp(privilege_, "read") == 0) {
        delete_read(exe_, target_);
    } else if (strcmp(privilege_, "open") == 0) {
        delete_open(exe_, target_);
    } else if (strcmp(privilege_, "creat") == 0) {
        delete_creat(exe_, target_);
    } else if (strcmp(privilege_, "chmod") == 0) {
        delete_chmod(exe_, target_);
    } else if (strcmp(privilege_, "mkdir") == 0) {
        delete_mkdir(exe_, target_);
    } else if (strcmp(privilege_, "rmdir") == 0) {
        delete_rmdir(exe_, target_);
    }
    return;
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
