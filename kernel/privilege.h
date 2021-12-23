/*
 * `privilege.h` declares arrays of privilege struct.
 */
#ifndef __KERNEL_PRIVILEGE_H__
#define __KERNEL_PRIVILEGE_H__

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define MAX_PRIVILEGE_NUM 128

struct Privilege {
    char exe[128];
    char target[128];
    int tombstone;
    int value;
};

static struct Privilege WritePrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege ReadPrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege OpenPrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege RmdirPrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege MkdirPrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege ChmodPrivilege[MAX_PRIVILEGE_NUM];
static struct Privilege CreatPrivilege[MAX_PRIVILEGE_NUM];

#endif // __KERNEL_PRIVILEGE_H__
