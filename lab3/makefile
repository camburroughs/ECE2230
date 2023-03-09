# The makefile for MP2.
#
# make          -- to build program lab3
# make design   -- to verify no illegal variables
# make driver   -- to compile testing program
# make clean    -- to delete object files, executable, and core
# make llist.o  -- to compile only llist.o
#
# You should not need to change this file.  
#
# Format for each entry
#    target : dependency list of targets or files
#    <tab> command 1
#    <tab> command 2
#    ...
#    <tab> last command
#    <blank line>   -- the list of commands must end with a blank line

lab2 : llist.o ids_support.o lab3.o
	gcc -Wall -g llist.o ids_support.o lab3.o -o lab3

llist.o : llist.c datatypes.h llist.h
	gcc -Wall -g -c llist.c

ids_support.o : ids_support.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c ids_support.c

lab2.o : lab2.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c lab3.c

driver : driver.o llist.o ids_support.o
	gcc -Wall -g llist.o ids_support.o driver.o -o driver

driver.o : driver.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c driver.c

#  @ prefix suppresses output of shell command
#  - prefix ignore errors
#  @command || true to avoid Make's error
#  : is shorthand for true
design :
	@grep -e "-> *ll_front" lab3.c ids_support.c ||:
	@grep -e "-> *ll_back" lab3.c ids_support.c ||:
	@grep -e "-> *ll_entry_count" lab3.c ids_support.c ||:
	@grep -e "-> *ll_sorted_state" lab3.c ids_support.c ||:
	@grep -e "-> *ll_next" lab3.c ids_support.c ||:
	@grep -e "-> *ll_prev" lab3.c ids_support.c ||:
	@grep -e "-> *data_ptr" lab3.c ids_support.c ||:
	@grep "compare_fun" lab3.c ids_support.c ||:
	@grep "llist_elem_t" lab3.c ids_support.c ||:
	@grep "generator_id" llist.c ||:
	@grep "dest_ip_addr" llist.c ||:
	@grep "alert_t" llist.c ||:
	@grep "ids_" llist.c ||:

clean :
	rm -f *.o lab3 driver core a.out

