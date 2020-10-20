#!/bin/bash
echo making object files and executables...
make &>> /dev/null

echo running valgrind...
valgrind --leak-check=full --show-leak-kinds=all crawler https://thayer.github.io/engs50/ test_dir 1 &> valout.txt
grep "ERROR SUMMARY\| Command: \| no leaks are possible" valout.txt

rm valout.txt
echo making clean...
make clean &>> /dev/null
