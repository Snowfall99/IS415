/*
 * listen daemon receives message sent from kernel through netlink and 
 * display them to user through stdout 
 */

#include <iostream>
#include "netlink.h"

int main() {
    char resp[1024];
    int ret;

    while (1) {
        ret = my_recv_msg(resp);
        if (ret == 0) {
            std::cout << resp << std::endl;
        } else {
            std::cerr << "failed to recvfrom netlink" << std::endl;
            break;
        }
    }
    return 1;
}