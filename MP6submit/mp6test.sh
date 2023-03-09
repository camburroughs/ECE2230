# mp6test.sh
# MP6 test script
# ECE 2230
# 
# Cameron L Burroughs
# burrou5
# C17134631
#

seeds="123 456 789 234 345 567 780 900 999 121"
Nvalues="10 15 20"
Rvalues="10 20 50 100"

echo "Show O(n^2) performance strongly connected graph"
    ./lab6 -g 3 -h 1 -n 11000 -s 0 -d 10999
    ./lab6 -g 3 -h 1 -n 5500 -s 0 -d 5499

echo "Show O(n^2) performance random graph"
    ./lab6 -g 4 -a 20 -h 1 -n 9700 -s 0 -d 9699
    ./lab6 -g 4 -a 20 -h 1 -n 5500 -s 0 -d 5499

echo "Show O(n^3) performance random graph, diameter"
    ./lab6 -g 4 -a 20 -h 2 -n 470 -s 0 -d 469 
    ./lab6 -g 4 -a 20 -h 2 -n 235 -s 0 -d 234

echo "Node density for random graphs" 

echo "not usually connected"
for seedNum in $seeds ; do
    ./lab6 -g 4 -h 2 -n 100 -r $seedNum -a 7
done

echo "usually connected"
for seedNum in $seeds ; do
    ./lab6 -g 4 -h 2 -n 100 -r $seedNum -a 20
done

echo "Multiple link-disjoint paths" 
for N in $Nvalues ; do
    ./lab6 -g 3 -h 3 -n $N -s 8 -d 9
done

echo "Multiple link-disjoint paths, random" 
for R in $Rvalues ; do
    ./lab6 -g 4 -h 3 -n 1000 -s 0 -d 999 -a $R
done 
echo "New seed" 
for R in $Rvalues ; do
    ./lab6 -g 4 -h 3 -n 1000 -s 0 -d 999 -a $R -r 123
done 

echo "print test"
    ./lab6 -g 1 -h 1 -r 123 -v
