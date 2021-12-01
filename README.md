**Firmiana**  
IS415 Group 8 Project, Fall 2021    
A CLI program for managing program's privileges based on system call overloading.

**Memebers**  
[JasmineChen123](https://github.com/JasmineChen123),
[Lajizz](https://github.com/Lajizz),
[Snowfall99](https://github.com/Snowfall99),
[Liangxiaoyou](https://github.com/liangxiaoyou)  

**Overview**  
/kernel: kernel module  
/user: user mode CLI program to manage privileges  
/evil: test program written in Rust  

**Build**  
Run the following instruction under the root directory
```
make
```

**Usage**  
// TODO(czx): to be implemented

**Roadmap**  
- CLI
[x] firmiana

- Netlink  
[x] netlink  

- System call
    - Oridinary system call  
        - [x] write  
        - [x] read  
        - [x] openat  
        - [x] creat  
        - [x] chmod  
        - [x] mkdir  
        - [x] rmdir

    - NetWork  
        - [ ] TBD(czx): have not decided yet
    
    - I/O
        - [ ] ioctl  
    
    - Root system call   
        - [ ] reboot

- Test  
    - [ ] Evil
    - [ ] Listening daemon


**TODO List**
1. an executable file for test (Evil, written in rust)
2. a listening daemon for display privileges (rust) 
3. fixup(core dumped twice, but I have no idea about the reason)
4. ioctl
5. net syscall
6. reboot
6. a more attractive CLI (use tmux for pre?)
8. whitelisting (bitmap)
