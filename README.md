**Firmiana**  
IS415 Group 2-1 Project, Fall 2021    
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
firmiana.sh: a simple shell script for presentation  

**Build**  
Run the following instruction under the root directory
```
make
```

**How to test**   
```
sudo insmod ./kernel/mycall.ko
./user/listen &
sudo firmia help
./firmiana.sh <subcommand>
sudo firmiana LIST
sudo rmmod mycall
```  

**Roadmap**  
- CLI
    - [x] firmiana

- Netlink  
    - [x] netlink  

- System call 
    - [x] creat  
    - [x] chmod  
    - [x] mkdir  
    - [x] rmdir
    - [x] write  
    - [x] read  
    - [x] openat 

- Test  
    - [x] Evil
    - [x] Listening daemon
