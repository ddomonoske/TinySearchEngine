#!/bin/bash


printf "\nDepth=3, Threads=1:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 5 ; } &>> speedTest.txt
printf "\nDepth=3, Threads=2:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 7 ; } &>> speedTest.txt
printf "\nDepth=3, Threads=3:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 9 ; } &>> speedTest.txt
printf "\nDepth=3, Threads=5:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 11 ; } &>> speedTest.txt
printf "\nDepth=3, Threads=10:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 13 ; } &>> speedTest.txt
printf "\nDepth=3, Threads=50:\n" &>> speedTest.txt
{ time multiCrawler https://thayer.github.io/engs50/ ../pagesThread 3 15 ; } &>> speedTest.txt


grep 'Depth\|real' speedTest.txt
rm speedTest.txt
