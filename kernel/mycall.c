#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/fdtable.h>
#include <linux/errno.h>
#include <linux/string.h>

#include "netlink.c"

char* my_get_type(char filename[]) {
    char* token = "";
    char* const delim = ".";
    token = strsep(&filename, delim);
    token = strsep(&filename, delim);
    return token;
}

struct privilege my_privilege = {"test", 0, 0, "txt"};

unsigned long *get_syscall_table(void)
{
    unsigned long base = kallsyms_lookup_name("sys_call_table");
    if (base == 0) 
    {
        return NULL;
    }
    return (unsigned long *)base;
}

void turn_on_wr_protect(unsigned long *table) 
{
    unsigned int level;
    pte_t *pte = lookup_address((long unsigned int)table, &level);
    pte->pte &= ~_PAGE_RW;
}

void turn_off_wr_protect(unsigned long *table)
{
    unsigned int level;
    pte_t *pte = lookup_address((long unsigned int)table, &level);
    pte->pte |= _PAGE_RW;
}

unsigned long *original_syscall_table = NULL;

typedef asmlinkage ssize_t (*original_syscall_t)(struct pt_regs* regs);
original_syscall_t original_read = NULL;
original_syscall_t original_write = NULL;

asmlinkage ssize_t hooked_sys_read(struct pt_regs* regs)
{
    int i;
    ssize_t ret_val = -1;
    ret_val = original_read(regs);
    for (i = 0; i < privilege_index; i++) {
        if (strcmp(current->comm, p[i].exe_file) == 0) {
            char path[100];
            char* ppath = path;
            struct files_struct *myfiles;
            myfiles = current->files;
            struct file *myfile;
            myfile = myfiles->fdt->fd[(unsigned int)regs->di];
            ppath = d_path(&(myfile->f_path), (char *)ppath, 100);
            char filename[100] = {'\0'};
            strcpy(filename, ppath);
            char* type = "";
            type = my_get_type(filename);
            if (type != NULL && strcmp(type, p[i].kind) == 0 && p[i].read == 0) {
                printk(KERN_INFO "%s cannot read type %s\n", p[i].exe_file, p[i].kind);
                char msg[64];
                sprintf(msg, "%s cannot write type %s\n", p[i].exe_file, p[i].kind);
                send_msg(msg, sizeof(msg));
                return -EPERM;
            }
        }
    }
    
    return ret_val;
}

asmlinkage ssize_t hooked_sys_write(struct pt_regs* regs) 
{
    int i;
    ssize_t ret_val = -1;
    for (i = 0; i < privilege_index; i++) {
        if (strcmp(current->comm, p[i].exe_file) == 0) {
            char path[100];
            char* ppath = path;
            struct files_struct *myfiles;
            myfiles = current->files;
            struct file *myfile;
            myfile = myfiles->fdt->fd[(unsigned int)regs->di];
            ppath = d_path(&(myfile->f_path), (char *)ppath, 100);
            char filename[100] = {'\0'};
            strcpy(filename, ppath);
            char* type = "";
            type = my_get_type(filename);
            if (type != NULL && strcmp(type, p[i].kind) == 0 && p[i].write == 0) {
                printk(KERN_INFO "%s cannot write type %s\n", p[i].exe_file, p[i].kind);
                char msg[64];
                sprintf(msg, "%s cannot write type %s\n", p[i].exe_file, p[i].kind);
                send_msg(msg, sizeof(msg));
                return -EPERM;
            } 
        }
    }
    ret_val = original_write(regs);
    return ret_val;
}

static int __init mycall_init(void)
{
    int nl = init_netlink();
    if (nl != 0) {
        return -1;
    }
    original_syscall_table = get_syscall_table();
    printk(KERN_INFO "syscall table: %px\n", original_syscall_table);
    
    turn_off_wr_protect(original_syscall_table);

    original_read = (original_syscall_t)original_syscall_table[__NR_read];
    original_write = (original_syscall_t)original_syscall_table[__NR_write];
    original_syscall_table[__NR_read] = (unsigned long)hooked_sys_read;
    original_syscall_table[__NR_write] = (unsigned long)hooked_sys_write;

    turn_on_wr_protect(original_syscall_table);
    return 0;
}

static void __exit mycall_exit(void)
{
    turn_off_wr_protect(original_syscall_table);
    original_syscall_table[__NR_read] = (unsigned long)original_read;
    original_syscall_table[__NR_write] = (unsigned long)original_write;
    turn_on_wr_protect(original_syscall_table);
    remove_netlink();
    printk(KERN_INFO "remove syscall\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snowfall99");
module_init(mycall_init);
module_exit(mycall_exit);
