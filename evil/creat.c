/*
 * creat is used for test creat privilege
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void creat_file(char * filename) {
    if(creat(filename, 0755)<0) {
        printf("create file %s failure!\n",filename);
        exit(EXIT_FAILURE);
    } else
        printf("create file %s success!\n", filename);
}

int main() {
    creat_file("new.txt");
}
