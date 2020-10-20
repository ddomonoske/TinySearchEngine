#!/bin/bash
echo making object files and executables...
make &>> /dev/null

mkdir test_dir
echo running valgrind...
valgrind --leak-check=full --show-leak-kinds=all crawler https://thayer.github.io/engs50/ test_dir 2

rm valout.txt
echo making clean...
make clean &>> /dev/null
