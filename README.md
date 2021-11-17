step 1: insmod mycall
$ cd IS412/kernel/
$ make
$ sudo dmesg -C
$ sudo insmod mycall.ko
$ dmesg

step 2: start cli
$ cd IS412/user/
$ make
$ sudo chmod 0700 ./cli
$ ./cli 

step 3: test
open a new terminal
$ cd IS412/test
$ make
$ sudo chmod 0700 in
$ sudo chmod 0700 out

$ ./out
$ ./in
$ dmesg

return to cli terminal
$ add in -t txt -w 0 -r 0
$ add out -t txt -w 0 -r 0

repeat ./in, ./out and dmesg and see what's different now

$ update in -t txt -r 1 -w 1
$ delete out -t txt

repeat again and see what's different

step 4: quit(in cli terminal)
$ exit
$ rmmod mycall
