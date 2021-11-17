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
static int privilege_index = 0;
static int total_count = 0;

// TODO
// auto find old privilege and update
// use total_count instead of privilege_index


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

// TODO 
// rename to update
// do not use index, use exe and type instead
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

// TODO
// use exe and type to find the privilege and change it tombstone 
void delete_privilege(char* exe_, char* type_, int index_) {
    strcpy(p[index_].exe, "");
    return;
}