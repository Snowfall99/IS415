#include <linux/string.h>
#include <linux/slab.h>

struct privilege {
    char exe_file[100];
    int write;
    int read;
    char kind[100];
};

static struct privilege p[128];//  = (struct privilege)kmalloc(128 * sizeof(struct privilege), GFP_KERNEL);
static int privilege_index = 0;

void add_privilege(char* exe_file_, int write_, int read_, char* kind_) {
    strcpy(p[privilege_index].exe_file, exe_file_);
    p[privilege_index].write = write_;
    p[privilege_index].read = read_;
    strcpy(p[privilege_index].kind, kind_);
    privilege_index ++;
    return;
}

void modify_privilege(char* exe_file_, int write_, int read_, char* kind_, int index_) {
    strcpy(p[index_].exe_file, exe_file_);
    p[index_].write = write_;
    p[index_].read = read_;
    strcpy(p[index_].kind, kind_);
    return;
}

void delete_privilege(char* exe_file_, int write_, int read_, char* kind_, int index_) {
    strcpy(p[index_].exe_file, "");
    return;
}