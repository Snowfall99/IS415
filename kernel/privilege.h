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
