#include "my_netlink.c"


int main(void) {
    int ret = -1;
    ret = mysendmsg("add test txt 1 0");
    return 0;
}