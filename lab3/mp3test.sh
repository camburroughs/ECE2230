#!/bin/sh
# mp3test.sh
# Cameron Burroughs
# burrou5
# ECE 2230, Fall 2020
#
#  
# A simple shell script for MP3 to generate data for performance analysis
# 
# This script tests 7 dfferent list sizes for each sort method and 
# starting list order. Each random list type requires at least one second
# to sort for at least one list size tested.
#
# On my system, it took about 32 seconds to run the script, given I am 
# testing 7 list sizes per sort, with the original script file, mine only 
# 13 seconds to run.
#
# This script was made executable using Option 1 shown below. 
#
# Option 1:  In a terminal type:
#      chmod +x mp3test.sh
#      ./mp3test.sh
#
# There are four tests for:
#    Insertion Sort, Recursive Selection Sort, Iterative Selection Sort,
#    and Merge Sort. 
#
# Each sort is run with random, ascending, and decending lists.
# 
# "sizes" is used to hold the size for each trial
#
# TEST 1 Insertion sort
echo "The date today is `date`"
echo "insertion sort with random list"
sizes="1000 4000 7000 10000 13000 16000 20000"
for listsize in $sizes ; do
   ./geninput $listsize 1 1 gen | ./lab3
done
echo "insertion sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 1 gen | ./lab3
done
echo "insertion sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 1 gen | ./lab3
done
#
# TEST 2 Recursive selection sort
echo "recursive selection sort with random list"
for listsize in $sizes ; do
   ./geninput $listsize 1 2 gen | ./lab3
done
echo "recursive selection sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 2 gen | ./lab3
done
echo "recursive selection sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 2 gen | ./lab3
done
#
# TEST 3 Iterative selection sort
echo "iterative selection sort with random list"
for listsize in $sizes ; do
   ./geninput $listsize 1 3 gen | ./lab3
done
echo "iterative selection sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 3 gen | ./lab3
done
echo "iterative selection sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 3 gen | ./lab3
done
#
# TEST 4 Merge sort
echo "merge sort with random list"
sizes="16000 100000 200000 300000 400000 500000 600000"
for listsize in $sizes ; do
   ./geninput $listsize 1 4 gen | ./lab3
done
echo "merge sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 4 gen | ./lab3
done
echo "merge sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 4 gen | ./lab3
done
#
echo "end"

