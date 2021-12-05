#!/bin/bash
echo -e "\033[32mIS415 Group 8:\033[0m \033[36mFirmiana\033[0m"
if [ $# -eq 2 ]
then 
  help
fi

function help() {
  echo -e "\033[33mUsage:\033[0m firmiana.sh <subcommand>"
}

function write_test() {
  echo -e "\033[32mTest write privilege\033[0m"
  cd ./evil
  echo "test.txt content: " | cat test.txt
  echo -e "\033[033mForbid\033[0m evil write to txt"
  firmiana ADD -e evil -t txt --write 0
  ./evil WRITE
  echo "test content: " | cat test.txt
  echo -e "\033[33mAllow\033[0m evil write to txt"
  firmiana UPDATE -e evil -t txt --write 1
  ./evil WRITE
  echo "test.txt content: " | cat test.txt
  echo -e "\033[32mWrite test finished\033[0m"
}

function read_test() {
  echo -e "\033[32mTest read privilege\033[0m"
  cd ./evil
  echo -e "\033[33mForbid\033[0m evil read txt"
  firmiana ADD -e evil -t txt --read 0
  ./evil READ
  echo -e "\033[33mAllow\033[0m evil read txt"
  firmiana UPDATE -e evil -t txt --read 1
  ./evil READ
  echo -e "\033[32mRead test finished\033[0m"
}

function open_test() {
  echo -e "\033[32mTest open privilege\033[0m"
  cd ./evil
  echo -e "\033[33mForbid\033[0m evil open test.txt"
  firmiana ADD -e evil -t test.txt --open 0
  ./evil OPEN
  echo -e "\033[33mAllow\033[0m evil open test.txt"
  firmiana UPDATE -e evil -t test.txt --open 1
  ./evil OPEN
  echo -e "\033[32mOpen test finished\033[0m"
}

function mkdir_test() {
  echo -e "\033[32mTest mkdir privilege\033[0m"
  cd ./evil
  pwd
  ls --color -la | grep new
  echo -e "\033[33mForbid\033[0m evil make directory"
  firmiana ADD -e evil --mkdir 0
  ./evil MKDIR
  ls --color -la | grep new
  echo -e "\033[33mAllow\033[0m evil make directory"
  firmiana UPDATE -e evil --mkdir 1
  ./evil MKDIR
  ls --color -la | grep new 
  echo -e "\033[32mMkdir test finished\033[0m"
}

function rmdir_test() {
  echo -e "\033[32mTest rmdir privilege\033[0m"
  cd ./evil 
  ls --color -la | grep new
  echo -e "\033[33mForbid\033[0m evil remove directory"
  firmiana ADD -e evil -t new --rmdir 0
  ./evil RMDIR 
  ls --color -la | grep new
  echo -e "\033[33mAllow\033[0m evil remove directory"
  firmiana UPDATE -e evil -t new --rmdir 1
  ./evil RMDIR
  ls --color -la | grep new 
  echo -e "\033[32mRmdir test finished\033[0m"
}

function chmod_test() {
  echo -e "\033[32mTest chmod privilege\033[0m"
  cd ./evil
  ls --color -la | grep test.txt
  echo -e "\033[33mForbid\033[0m evil change mode of txt"
  firmiana ADD -e evil -t txt --chmod 0
  ./evil CHMOD
  ls --color -la | grep test.txt
  echo -e "\033[33mAllow\033[0m evil change mode of txt"
  firmiana UPDATE -e evil -t txt --chmod 1
  ./evil CHMOD
  ls --color -la | grep test.txt
  echo -e "\033[32mChmod test finished\033[0m"
}

function creat_test() {
  echo -e "\033[32mTest creat privilege\033[0m"
  cd ./evil
  ls --color
  echo -e "\033[33mForbid\033[0m create txt file"
  firmiana ADD -e creat -t txt --creat 0
  ./creat
  ls --color
  echo -e "\033[33mAllow\033[0m create txt file"
  firmiana UPDATE -e creat -t txt --creat 1
  ./creat
  ls --color
  echo -e "\033[32mCreat test finished\033[0m"
}

case $1 in
  WRITE) write_test ;;
  READ) read_test ;;
  OPEN) open_test ;;
  MKDIR) mkdir_test ;;
  RMDIR) rmdir_test ;;
  CHMOD) chmod_test ;;
  CREAT) creat_test ;;
  *) help ;;
esac