#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define MAX_PRIVILEGE_NUM 128

struct privilege {
    char exe[100];
    char type[100];
    int write;
    int read;
    int tombstone;
};

static struct privilege p[MAX_PRIVILEGE_NUM];
static int total_count = 0;

void add_privilege(char*, char*, int, int);
void update_privilege(char*, char*, int, int);
void delete_privilege(char*, char*);
void list_privilege(void);

void add_privilege(char* exe_, char* type_, int write_, int read_) {
    int i;
    int flag = 1;
    // check whether there is a duplicated privilege
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(p[i].exe, exe_) == 0 && strcmp(p[i].type, type_) == 0 && p[i].tombstone) {
            flag = 0;
            break;
        }
    }
    if (!flag) {
        printk(KERN_INFO "there is a duplicated privilege\n");
        return;
    }

    // find the first slot to add privilege
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!p[i].tombstone) {
            break;
        }
    }
    strcpy(p[i].exe, exe_);
    strcpy(p[i].type, type_);
    p[i].tombstone = 1;
    p[i].write = write_;
    p[i].read = read_;
    total_count ++;
    return;
}

void update_privilege(char* exe_, char* type_, int write_, int read_) {
    int i;
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(p[i].exe, exe_) == 0 && strcmp(p[i].type, type_) == 0 && p[i].tombstone) {
            p[i].write = write_;
            p[i].read = read_;
            break;
        }
    }
    return;
}

void delete_privilege(char* exe_, char* type_) {
    int i;
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (strcmp(p[i].exe, exe_) == 0 && strcmp(p[i].type, type_) == 0 && p[i].tombstone) {
            p[i].tombstone = 0;
            total_count --;
            break;
        }
    }
    return;
}

// TODO
// list privilege is under construction
// netlink and privilege is close coupling
void list_privilege(void) {
    return;
}