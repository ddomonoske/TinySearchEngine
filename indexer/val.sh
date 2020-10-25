#!/bin/bash
echo making object files and executables...
make &>> /dev/null

mkdir test_dir
echo running valgrind...
valgrind --leak-check=full --show-leak-kinds=all indexer

echo making clean...
make clean &>> /dev/null
