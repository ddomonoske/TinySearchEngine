#!/bin/bash
echo making object files and executables...
make &>> /dev/null

echo running valgrind...; echo
valgrind --leak-check=full --show-leak-kinds=all indexer ../pages indexName1 #&> memcheck.txt
#grep "ERROR SUMMARY\| Command: \| no leaks are possible" memcheck.txt; echo
valgrind --leak-check=full --show-leak-kinds=all indexer_test indexName1 indexName2 #&> memcheck.txt
#grep "ERROR SUMMARY\| Command: \| no leaks are possible" memcheck.txt; echo
valgrind --leak-check=full --show-leak-kinds=all indexer_test badName indexName4 #&> memcheck.txt
#grep "ERROR SUMMARY\| Command: \| no leaks are possible" memcheck.txt; echo

rm memcheck.txt
echo making clean...
make clean &>> /dev/null
