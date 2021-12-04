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
0. Kernel module:  
```
cd kernel
sudo insmod mycall.ko
sudo rmmod mycall
```  
1. Firmiana:  
```
cd firmiana
sudo cp firmiana /usr/bin/firmiana
firmiana help
firmiana <subcommand> <flags> 
```
2. Evil:  
```
cd evil
cargo run <subcommand>
```

**Roadmap**  
- CLI
    - [x] firmiana

- Netlink  
    - [x] netlink  

- System call
    - File 
        - [x] creat  
        - [x] chmod  
        - [x] mkdir  
        - [x] rmdir

    - NetWork  
        - [ ] TBD(czx): have not decided yet
    
    - I/O
        - [x] write  
        - [x] read  
        - [x] openat 
    
    - Root system call   
        - [ ] reboot

- Test  
    - [x] Evil
    - [x] Listening daemon

**TODO List**
1. a more attractive CLI (use tmux for pre?)
2. whitelisting (bitmap)
