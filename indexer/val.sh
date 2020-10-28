#!/bin/bash
echo making object files and executables...
make &>> /dev/null

echo running valgrind...
valgrind --leak-check=full --show-leak-kinds=all indexer 2 &> memcheck.txt
grep "ERROR SUMMARY\| Command: \| no leaks are possible" memcheck.txt; echo
valgrind --leak-check=full --show-leak-kinds=all indexer_test &> memcheck.txt
grep "ERROR SUMMARY\| Command: \| no leaks are possible" memcheck.txt; echo

rm memcheck.txt
echo making clean...
make clean &>> /dev/null
