#!/bin/bash
echo making object files and executables...
make &>> /dev/null

mkdir ../pagesThread
echo running valgrind...
valgrind --leak-check=full --show-leak-kinds=all multiCrawler https://thayer.github.io/engs50/ ../pagesThread 1 10

#multiCrawler https://thayer.github.io/engs50/ ../pagesThread 1 &> multiResult.txt
#crawler https://thayer.github.io/engs50/ ../pagesThread 1 &> normResult.txt

#echo making clean...
#make clean &>> /dev/null
