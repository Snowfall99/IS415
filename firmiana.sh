#!/bin/bash
echo "IS415 Group 8: Firmiana"
if [ $# -eq 2 ]
then 
  echo "Usage: firmiana.sh <subcommand>"
fi

function write_test() {
  echo "Test write privilege"
  cd ./evil
  echo "test.txt content"
  cat test.txt
  echo "Forbid evil write to txt"
  firmiana ADD -e evil -t txt --write 0
  ./evil WRITE
  echo "test content"
  cat test.txt
  echo "Allow evil write to txt"
  firmiana UPDATE -e evil -t txt --write 1
  ./evil WRITE
  echo "test.txt content"
  cat test.txt
  echo "Write test finished"
}

function read_test() {
  echo "Test read privilege"
  cd ./evil
  echo "Forbid evil read txt"
  firmiana ADD -e evil -t txt --read 0
  ./evil READ
  echo "Allow evil read txt"
  firmiana UPDATE -e evil -t txt --read 1
  ./evil READ
  echo "Read test finished"
}

function open_test() {
  echo "Test open privilege"
  cd ./evil
  echo "Forbid evil open test.txt"
  firmiana ADD -e evil -t test.txt --open 0
  ./evil OPEN
  echo "Allow evil open test.txt"
  firmiana UPDATE -e evil -t test.txt --open 1
  ./evil OPEN
  echo "Open test finished"
}

function mkdir_test() {
  echo "Test mkdir privilege"
  cd ./evil
  ls -la | grep new
  echo "Forbid evil make directory"
  firmiana ADD -e evil -t ./new --mkdir 0
  ./evil MKDIR
  ls -la | grep new
  echo "Allow evil make directory"
  firmiana UPDATE -e evil -t ./new --mkdir 1
  ./evil MKDIR
  ls -la | grep new 
  echo "Mkdir test finished"
}

function rmdir_test() {
  echo "Test rmdir privilege"
  cd ./evil 
  ls -la | grep new
  echo "Forbid evil remove directory"
  firmiana ADD -e evil -t ./new --rmdir 0
  ./evil RMDIR 
  ls -la | grep new
  echo "Allow evil remove directory"
  firmiana UPDATE -e evil -t ./new --rmdir 1
  ./evil RMDIR
  ls -la | grep new 
  echo "Rmdir test finished"
}

function chmod_test() {
  echo "Test chmod privilege"
  cd ./evil
  ls -la | grep test.txt
  echo "Forbid evil change mode of txt"
  firmiana ADD -e evil -t txt --chmod 0
  ./evil CHMOD
  ls -la | grep test.txt
  echo "Allow evil change mode of txt"
  firmiana UPDATE -e evil -t txt --chmod 1
  ./evil CHMOD
  ls -la | grep test.txt
  echo "Chmod test finished"
}

function creat_test() {
  echo "Test creat privilege"
  cd ./evil
  ls | grep new.txt
  echo "Forbid create txt file"
  firmiana ADD -e creat -t txt --creat 0
  ./creat
  ls | grep new.txt
  echo "Allow create txt file"
  firmiana UPDATE -e creat -t txt --creat 1
  ./creat
  ls | grep new.txt
  echo "Creat test finished"
}

case $1 in
  WRITE) write_test ;;
  READ) read_test ;;
  OPEN) open_test ;;
  MKDIR) mkdir_test ;;
  RMDIR) rmdir_test ;;
  CHMOD) chmod_test ;;
  CREAT) creat_test ;;
esac