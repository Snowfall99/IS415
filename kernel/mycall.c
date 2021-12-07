#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/fdtable.h>
#include <linux/errno.h>
#include <linux/string.h>

#include "netlink.h"

// get type from filename
char* my_get_type(char filename[]) {
    char* token = "";
    char* const delim = ".";
    token = strsep(&filename, delim);
    token = strsep(&filename, delim);
    return token;
}

unsigned long *get_syscall_table(void) {
    unsigned long base = kallsyms_lookup_name("sys_call_table");
    if (base == 0) {
        return NULL;
    }
    return (unsigned long *)base;
}

// turn on write protection bit of syscall table
void turn_on_wr_protect(unsigned long *table) {
    unsigned int level;
    pte_t *pte = lookup_address((long unsigned int)table, &level);
    pte->pte &= ~_PAGE_RW;
}

// turn off write protection bit of syscall table
void turn_off_wr_protect(unsigned long *table) {
    unsigned int level;
    pte_t *pte = lookup_address((long unsigned int)table, &level);
    pte->pte |= _PAGE_RW;
}

unsigned long *original_syscall_table = NULL;

typedef asmlinkage ssize_t (*original_syscall_t)(struct pt_regs* regs);
original_syscall_t original_read = NULL;
original_syscall_t original_write = NULL;
original_syscall_t original_openat = NULL;
original_syscall_t original_mkdir = NULL;
original_syscall_t original_rmdir = NULL;
original_syscall_t original_creat = NULL;
original_syscall_t original_chmod = NULL;
original_syscall_t original_rename = NULL;

asmlinkage ssize_t hooked_sys_read(struct pt_regs* regs) {
    int i;
    ssize_t ret_val = -1;
    char path[100];
    char* ppath = path;
    struct files_struct *myfiles;
    struct file *myfile;
    char filename[100] = {'\0'};
    char* type = "";
    // char msg[64];

    ret_val = original_read(regs);
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!ReadPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, ReadPrivilege[i].exe) == 0) {
            myfiles = current->files;
            myfile = myfiles->fdt->fd[(unsigned int)regs->di];
            ppath = d_path(&(myfile->f_path), (char *)ppath, 100);
            strcpy(filename, ppath);
            type = getType(filename);
            if (type != NULL && strcmp(type, ReadPrivilege[i].target) == 0 && ReadPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot read type %s\n", ReadPrivilege[i].exe, ReadPrivilege[i].target);
                // sprintf(msg, "%s cannot write type %s", ReadPrivilege[i].exe, ReadPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return ret_val;
}

asmlinkage ssize_t hooked_sys_write(struct pt_regs* regs) {
    int i;
    ssize_t ret_val = -1;
    char path[100];
    char* ppath = path;
    struct files_struct *myfiles;
    struct file *myfile;
    char filename[100] = {'\0'};
    char* type = "";
    // char msg[64];

    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!WritePrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, WritePrivilege[i].exe) == 0) {
            myfiles = current->files;
            myfile = myfiles->fdt->fd[(unsigned int)regs->di];
            ppath = d_path(&(myfile->f_path), (char *)ppath, 100);
            strcpy(filename, ppath);
            type = getType(filename);
            if (type != NULL && strcmp(type, WritePrivilege[i].target) == 0 && WritePrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot write type %s\n", WritePrivilege[i].exe, WritePrivilege[i].target);
                // sprintf(msg, "%s cannot write type %s", WritePrivilege[i].exe, WritePrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            } 
        }
    }
    ret_val = original_write(regs);
    return ret_val;
}

asmlinkage ssize_t hooked_sys_openat(struct pt_regs* regs) {
    char* filename = (char *)regs->si;
    char* kbuf;
    long error;
    int i;
    // char msg[64];

    kbuf = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf == NULL) {
        return original_openat(regs);
    }
    error = copy_from_user(kbuf, filename, NAME_MAX);
    if (error) {
        return original_openat(regs);
    }
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!OpenPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, OpenPrivilege[i].exe) == 0) {
            if (memcmp(kbuf, OpenPrivilege[i].target , strlen(OpenPrivilege[i].target)) == 0 && OpenPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot open file %s\n", OpenPrivilege[i].exe, OpenPrivilege[i].target);
                // sprintf(msg, "%s cannot open file %s", OpenPrivilege[i].exe, OpenPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return original_openat(regs);
}

asmlinkage ssize_t hooked_sys_mkdir(struct pt_regs* regs) {
    char* pathname = (char*)regs->di;
    char* kbuf;
    long error;
    int i;
    // char msg[64];

    kbuf = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf == NULL) {
        return original_mkdir(regs);
    }
    error = copy_from_user(kbuf, pathname, NAME_MAX);
    if (error) {
        return original_mkdir(regs);
    }
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!MkdirPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, MkdirPrivilege[i].exe) == 0) {
            if (MkdirPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot make dir %s\n", MkdirPrivilege[i].exe, MkdirPrivilege[i].target);
                // sprintf(msg, "%s cannot make dir %s", MkdirPrivilege[i].exe, MkdirPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return original_mkdir(regs);
}

asmlinkage ssize_t hooked_sys_rmdir(struct pt_regs* regs) {
    char* pathname = (char*)regs->di;
    char* kbuf;
    long error;
    int i;
    // char msg[64];

    kbuf = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf == NULL) {
        return original_rmdir(regs);
    }
    error = copy_from_user(kbuf, pathname, NAME_MAX);
    if (error) {
        return original_rmdir(regs);
    }
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!RmdirPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, RmdirPrivilege[i].exe) == 0) {
            if (RmdirPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot remove dir %s\n", RmdirPrivilege[i].exe, RmdirPrivilege[i].target);
                // sprintf(msg, "%s cannot remove dir %s", RmdirPrivilege[i].exe, RmdirPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return original_rmdir(regs);
}

asmlinkage ssize_t hooked_sys_creat(struct pt_regs* regs) {
    char* pathname = (char*)regs->di;
    char* kbuf;
    long error;
    int i;
    char* type = "";
    // char msg[64];

    kbuf = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf == NULL) {
        return original_creat(regs);
    }
    error = copy_from_user(kbuf, pathname, NAME_MAX);
    if (error) {
        return original_creat(regs);
    }
    for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!CreatPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, CreatPrivilege[i].exe) == 0) {
            type = getType(kbuf);
            if (type != NULL && strcmp(type, CreatPrivilege[i].target) == 0 && CreatPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot creat type %s\n", CreatPrivilege[i].exe, CreatPrivilege[i].target);
                // sprintf(msg, "%s cannot creat type %s", CreatPrivilege[i].exe, CreatPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return original_creat(regs);
}

asmlinkage ssize_t hooked_sys_chmod(struct pt_regs* regs) {
    char* pathname = (char*)regs->di;
    char* kbuf;
    long error;
    int i;
    char* type = "";
    // char msg[64];

    kbuf = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf == NULL) {
        return original_chmod(regs);
    }
    error = copy_from_user(kbuf, pathname, NAME_MAX);
    if (error) {
        return original_chmod(regs);
    }
     for (i = 0; i < MAX_PRIVILEGE_NUM; i++) {
        if (!ChmodPrivilege[i].tombstone) {
            continue;
        }
        if (strcmp(current->comm, ChmodPrivilege[i].exe) == 0) {
            type = getType(kbuf);
            if (type != NULL && strcmp(type, ChmodPrivilege[i].target) == 0 && ChmodPrivilege[i].value == 0) {
                printk(KERN_INFO "%s cannot chmod %s\n", ChmodPrivilege[i].exe, ChmodPrivilege[i].target);
                // sprintf(msg, "%s cannot chmod %s", ChmodPrivilege[i].exe, ChmodPrivilege[i].target);
                // sendMsg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    return original_chmod(regs);
}

asmlinkage ssize_t hooked_sys_rename(struct pt_regs* regs) {
    char* filename1 = (char*)regs->dx;
    char* filename2 = (char*)regs->cx;
    char* kbuf1;
    char* kbuf2;
    long error;

    kbuf1 = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf1 == NULL) {
        return original_rename(regs);
    }
    kbuf2 = kmalloc(NAME_MAX, GFP_KERNEL);
    if (kbuf2 == NULL) {
        return original_rename(regs);
    }
    error = copy_from_user(kbuf1, filename1, NAME_MAX);
    if (error) {
        return original_rename(regs);
    }
    error = copy_from_user(kbuf2, filename2, NAME_MAX);
    if (error) {
        return original_rename(regs);
    }
    printk(KERN_INFO "filename1: %s", kbuf1);
    printk(KERN_INFO "filename2: %s", kbuf2);
    return original_rename(regs);
}

static int __init mycall_init(void) {
    int nl = init_netlink();
    if (nl != 0) {
        return -1;
    }
    original_syscall_table = get_syscall_table();
    printk(KERN_INFO "syscall table: %px\n", original_syscall_table);
    turn_off_wr_protect(original_syscall_table);
    original_read = (original_syscall_t)original_syscall_table[__NR_read];
    original_write = (original_syscall_t)original_syscall_table[__NR_write];
    original_openat = (original_syscall_t)original_syscall_table[__NR_openat];
    original_mkdir = (original_syscall_t)original_syscall_table[__NR_mkdir];
    original_rmdir = (original_syscall_t)original_syscall_table[__NR_rmdir];
    original_creat = (original_syscall_t)original_syscall_table[__NR_creat];
    original_chmod = (original_syscall_t)original_syscall_table[__NR_chmod];
    original_rename = (original_syscall_t)original_syscall_table[__NR_rename];
    original_syscall_table[__NR_read] = (unsigned long)hooked_sys_read;
    original_syscall_table[__NR_write] = (unsigned long)hooked_sys_write;
    original_syscall_table[__NR_openat] = (unsigned long)hooked_sys_openat;
    original_syscall_table[__NR_mkdir] = (unsigned long)hooked_sys_mkdir;
    original_syscall_table[__NR_rmdir] = (unsigned long)hooked_sys_rmdir;
    original_syscall_table[__NR_creat] = (unsigned long)hooked_sys_creat;
    original_syscall_table[__NR_chmod] = (unsigned long)hooked_sys_chmod;
    original_syscall_table[__NR_rename] = (unsigned long)hooked_sys_rename;
    turn_on_wr_protect(original_syscall_table);
    return 0;
}

static void __exit mycall_exit(void) {
    turn_off_wr_protect(original_syscall_table);
    original_syscall_table[__NR_read] = (unsigned long)original_read;
    original_syscall_table[__NR_write] = (unsigned long)original_write;
    original_syscall_table[__NR_openat] = (unsigned long)original_openat;
    original_syscall_table[__NR_mkdir] = (unsigned long)original_mkdir;
    original_syscall_table[__NR_rmdir] = (unsigned long)original_rmdir;
    original_syscall_table[__NR_creat] = (unsigned long)original_creat;
    original_syscall_table[__NR_chmod] = (unsigned long)original_chmod;
    original_syscall_table[__NR_rename] = (unsigned long)original_rename;
    turn_on_wr_protect(original_syscall_table);
    remove_netlink();
    printk(KERN_INFO "remove syscall\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snowfall99");
module_init(mycall_init);
module_exit(mycall_exit);
