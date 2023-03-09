# MP7 test script
# ECE 2230
# 
# Cameron L Burroughs
# burrou5
# C17134631
#

alpha="0.5 0.75 0.9 0.95 0.99"

echo "testing linear probing retrieve driver"
for alp in $alpha ; do
	./lab7 -r -m 65537 -a $alp -i rand
done

echo "testing double hashing retrieve driver"
for alp in $alpha ; do
	./lab7 -r -m 65537 -a $alp -i rand -h double
done

echo "testing seperate chaining retrieve driver"
for alp in $alpha ; do
	./lab7 -r -m 65537 -a $alp -i rand -h chain
done

echo "part 2 random lists"
	./lab7 -r -m 65537 -a 0.85 -i rand 
	./lab7 -r -m 65537 -a 0.85 -i rand -h double
	./lab7 -r -m 65537 -a 0.85 -i rand -h chain

echo "part 2 sequential lists"
	./lab7 -r -m 65537 -a 0.85 -i seq 
	./lab7 -r -m 65537 -a 0.85 -i seq -h double
	./lab7 -r -m 65537 -a 0.85 -i seq -h chain

echo "part 3 equilibrium driver test"
	./lab7 -e -m 65537 -t 50000
	./lab7 -e -m 65537 -t 100000
	./lab7 -e -m 65537 -t 50000 -h double
	./lab7 -e -m 65537 -t 100000 -h double
	./lab7 -e -m 65537 -t 50000 -h chain
	./lab7 -e -m 65537 -t 100000 -h chain
echo "part 4 memory"
	valgrind --leak-check=yes ./lab7 -r 
	valgrind --leak-check=yes ./lab7 -r -h double
	valgrind --leak-check=yes ./lab7 -r -h chain

