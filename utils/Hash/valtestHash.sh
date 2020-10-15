#!/bin/bash
echo making object files and executables...
make &>> /dev/null

echo running valgrind...
valgrind --leak-check=full test1to3 &>> memcheck.txt
grep "ERROR SUMMARY\| Command: " memcheck.txt; > memcheck.txt
valgrind --leak-check=full testHapply &>> memcheck.txt
grep "ERROR SUMMARY\| Command: " memcheck.txt; > memcheck.txt
valgrind --leak-check=full testHsearch &>> memcheck.txt
grep "ERROR SUMMARY\| Command: " memcheck.txt; > memcheck.txt
valgrind --leak-check=full testHremove &>> memcheck.txt
grep "ERROR SUMMARY\| Command: " memcheck.txt; > memcheck.txt

rm memcheck.txt
echo making clean...
make clean &>> /dev/null
