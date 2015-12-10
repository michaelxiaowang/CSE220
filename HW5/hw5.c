/*
Homework #5
name: MICHAEL_WANG
sbuid: 109055666
*/


#include <stdlib.h>
#include <stdio.h>

/* Function prototypes */
int promptForId(void);
int scrambleId(int id);
int xor(int v, int id);
int shift(int v, int n);
int printResults(int original, int scrambled, int result);

/* Ask the user for their SBUID */
int promptForId(void) {
	int id, valid;
	/* Loop until valid input is given */
	do {
		char line[256];
		printf("Please enter your SBUID: ");
		/* Read in the whole line */
		fgets(line, sizeof(line), stdin);
		/* If we were not able to parse the string, tell the user */
		if(!(valid = sscanf(line, "%d", &id))) {
			printf("Invalid id entered.\n");
		}
	} while(!valid);
	return id;
}

/* Scramble the SBUID */
int scrambleId(int id) {
	int scrambled = id, i, r, rounds;
	srand(id); 
	r = rand();
	/* Bound this random number between 1-10 */
	rounds = (rand() % 10) + 1;
	printf("Performing %d rounds of shifting and xoring.\n", rounds);
	/* Go through some crazy shifting operations */
	for(i = 0; i < rounds; i++) {
		scrambled = shift(scrambled, r >>= 1);
	}
	return scrambled;
}

/* Perform bitwise XOR on the SBU ID */
int xor(int v, int id) {
	int result = 0;
	result = v ^ id;
	return result;
}

/* Shift the value right and xor */
int shift(int v, int r) {
	int shamt = 0x08000000;
	int result = 0;
	while(shamt) {
		result += xor(v, r); /* add the return value of xor to result */
		shamt = shamt >> 1; /* Shift right by 1 */
	}
	return result;
}

int printResults(int original, int scrambled, int result) {
	/* If the scrambled number is even */
	int isEven;
	printf("%d -> %d\n", original, scrambled);
	isEven = scrambled % 2; 
	if(isEven == 0)
		printf("The scrambled result is even!\n");
		return result;
	/* else it is odd */
	printf("The scrambled result is odd!\n");
	return result;
}

/* The program starts here */
int main() {
	int id, scrambled, result = 0;
	id = promptForId();
	printf("SBUID: %d\n", id);
	/* Scramble the SBUID */
	scrambled = scrambleId(id);
	/* Print out the scrambled value */
	result = printResults(id, scrambled, result);
	return result;
}

/* @@GDB_OUTPUT_HERE@@

GNU gdb (GDB) 7.8
Copyright (C) 2014 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "sparc-sun-solaris2.10".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from a.out...done.
(gdb) break 28
Breakpoint 1 at 0x10c08: file hw5.c, line 28.
(gdb) break 45
Breakpoint 2 at 0x10cf8: file hw5.c, line 45.
(gdb) break 53
Breakpoint 3 at 0x10d58: file hw5.c, line 53.
(gdb) run
Starting program: /export/home2/m/i/micwang/a.out 
[Thread debugging using libthread_db enabled]
[New Thread 1 (LWP 1)]
Please enter your SBUID: 109055666
[Switching to Thread 1 (LWP 1)]

Breakpoint 1, promptForId () at hw5.c:28
28                      if(!(valid = sscanf(line, "%d", &id))) {
(gdb) print line
$1 = "109055666\n", '\000' <repeats 46 times>, "\377\377\377\377\377\377\377\377\377:\000\000\377\277\373\330\377:@L\000\000\000\f\000\001\000\064\377\277\377\325\377\377\377\377\000\000\000\000\377\277\374\304\000\000\000\002\377\377\377\377\377\377\377\377\377\277\373\340\377:X\350\000\000\000\000\000\000\000\000\377\277\375\020\000\000\000\000\000\000\000\001\000\001\005\v\377\067\np\377 \267\200\000\001\005\v\377\067\004 \377\067\004 \000\000\000\002\006\212\317\004\000\000\000\021\000\001\003L\025\001\377\377\000\001\000\000\000\000\000\000\000\000\000\002\377\277\373\330"...
(gdb) continue
Continuing.
SBUID: 109055666
Performing 2 rounds of shifting and xoring.

Breakpoint 2, scrambleId (id=109055666) at hw5.c:45
45                      scrambled = shift(scrambled, r >>= 1);
(gdb) print i
$2 = 0
(gdb) continue
Continuing.

Breakpoint 3, xor (v=109055666, id=11350) at hw5.c:53
53              result = v ^ id;
(gdb) backtrace
#0  xor (v=109055666, id=11350) at hw5.c:53
#1  0x00010da8 in shift (v=109055666, r=11350) at hw5.c:62
#2  0x00010d14 in scrambleId (id=109055666) at hw5.c:45
#3  0x00010e8c in main () at hw5.c:87
(gdb) info frame
Stack level 0, frame at 0xffbffb18:
 pc = 0x10d58 in xor (hw5.c:53); saved pc = 0x10da8
 called by frame at 0xffbffb80
 source language c.
 Arglist at 0xffbffb18, args: v=109055666, id=11350
 Locals at 0xffbffb18, Previous frame's sp in fp
 Saved registers:
  l0 at 0xffbffb18, l1 at 0xffbffb1c, l2 at 0xffbffb20, l3 at 0xffbffb24,
  l4 at 0xffbffb28, l5 at 0xffbffb2c, l6 at 0xffbffb30, l7 at 0xffbffb34,
  i0 at 0xffbffb38, i1 at 0xffbffb3c, i2 at 0xffbffb40, i3 at 0xffbffb44,
  i4 at 0xffbffb48, i5 at 0xffbffb4c, fp at 0xffbffb50, i7 at 0xffbffb54
(gdb) clear 53
Deleted breakpoint 3 
(gdb) continue
Continuing.

Breakpoint 2, scrambleId (id=109055666) at hw5.c:45
45                      scrambled = shift(scrambled, r >>= 1);
(gdb) print 1
$3 = 1
(gdb) continue
Continuing.
109055666 -> -395722764
The scrambled result is even!
[Inferior 1 (process 16038    ) exited normally]
(gdb) quit

@@GDB_OUTPUT_HERE@@ */