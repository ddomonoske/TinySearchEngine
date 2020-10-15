#!/bin/bash

touch memcheck.txt
valgrind --leak-check=full testPut &>> memcheck.txt
valgrind --leak-check=full testGet &>> memcheck.txt
valgrind --leak-check=full testApply &>> memcheck.txt
valgrind --leak-check=full testRemove &>> memcheck.txt

grep "ERROR SUMMARY\| Command: " memcheck.txt
rm memcheck.txt
