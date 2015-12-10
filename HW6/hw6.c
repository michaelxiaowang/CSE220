/*
Homework #6
name: MICHAEL_WANG
sbuid: 109055666
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ERROR_REG 1
#define ERROR_INSTR 2
#define ERROR_IMMEDIATE 3
#define ERROR_FLAGS 4

void printmenu(char *const *argv);

int main(int argc, char *argv[]) {
	int hflag = 0, iflag = 0, rflag = 0, mflag = 0, uflag = 0, opt = 0;
	char buffer[1024];
	char line[1024];
	unsigned int instruction;
	unsigned int checker;
	int i = 0, total = 0;
	int output[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int rtype[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int itype[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int jtype[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	/*Check to see which flags are used, if -h flag found, print menu. If invalid flag found, exit.*/
	while((opt=getopt(argc, argv, "himru")) != -1) {
		switch(opt) {
			case 'h':
				hflag = 1;
				printmenu(argv);
				return EXIT_SUCCESS;
			case 'i':
				iflag = 1;
				break;
			case 'm':
				mflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			case 'u':
				uflag = 1;
				break;
			default:
				return ERROR_FLAGS;
				break;
		}		
	}

	/*Tests whether invalid flag combinations are present*/
	if(uflag ==1 && (iflag == 0 && mflag == 0 && rflag ==0)) {
		printmenu(argv);
		return ERROR_FLAGS;
	}
	if(iflag == 1 && (mflag == 1 || rflag == 1)) {
		printmenu(argv);
		return ERROR_FLAGS;
	}
	if(mflag == 1 && (iflag == 1 || rflag == 1)) {
		printmenu(argv);
		return ERROR_FLAGS;
	}
	if(rflag == 1 && (mflag == 1 || iflag == 1)) {
		printmenu(argv);
		return ERROR_FLAGS;
	}
	if(hflag == 0 && iflag == 0 && mflag == 0 && rflag == 0 && uflag == 0) {
		printmenu(argv);
		return ERROR_FLAGS;
	}

	/*Since immediate values are printed until we see an error, we can print as soon as flags say so*/
	if(mflag == 1 && uflag == 1)
			printf("IMMEDIATE-VALUE\n");


	while(fgets(buffer, 1024, stdin)) {

		/*This section checks for invalid hex instructions and returns the right error flag.*/
		if(!((sscanf(buffer, "%s", line)) == 1)) {
			if(iflag == 1)
				return ERROR_INSTR;
			if(rflag == 1)
				return ERROR_REG;
			if(mflag == 1)
				return ERROR_IMMEDIATE;
		}
		if(strlen(line) != 10) {
			if(iflag == 1)
				return ERROR_INSTR;
			if(rflag == 1)
				return ERROR_REG;
			if(mflag == 1)
				return ERROR_IMMEDIATE;
		}
		for(i = 2; i <= 10; i++) {
			if(line[i] == 'x') {
				if(iflag == 1)
					return ERROR_INSTR;
				if(rflag == 1)
					return ERROR_REG;
				if(mflag == 1)
					return ERROR_IMMEDIATE;
			}
		}
		if(!(line[0] == '0' && line[1] == 'x')) {
			if(iflag == 1)
				return ERROR_INSTR;
			if(rflag == 1)
				return ERROR_REG;
			if(mflag == 1)
				return ERROR_IMMEDIATE;
		}
		if(!(sscanf(line, "%x", &instruction) == 1)) {
			if(iflag == 1)
				return ERROR_INSTR;
			if(rflag == 1)
				return ERROR_REG;
			if(mflag == 1)
				return ERROR_IMMEDIATE;
		}

		/*When the section is correct, do work for the output*/
		if(mflag == 1)
		{
			checker = instruction;
			checker = checker >> 26;
			checker = checker & 63;
			if(checker != 0)
			{
				if(checker == 2 || checker == 3)
				{
					checker = instruction & 67108863;
				}
				else
				{
					checker = instruction & 65535;
				}
				printf("0x%x\n", checker);
			}
		}

		if(iflag == 1)
		{
			checker = instruction;
			checker = checker >> 26;
			checker = checker & 63;
			if(checker == 0)
				output[0]++;
			else if(checker == 2 || checker == 3)
				output[2]++;
			else
				output[1]++;
			total++;
		}

		if(rflag == 1) {
			checker = instruction;
			checker = checker >> 26;
			checker = checker & 63;
			if(!(checker == 2 || checker == 3)) {
				if(checker == 0) {
					checker = instruction;
					checker = checker >> 11;
					checker = checker & 31;
					output[checker]++;
					total++;
					rtype[checker]++;
					checker = instruction;
					checker = checker >> 16;
					checker = checker & 31;
					output[checker]++;
					total++;
					rtype[checker]++;
					checker = instruction;
					checker = checker >> 21;
					checker = checker & 31;
					output[checker]++;
					total++;
					rtype[checker]++;
				}
				else {
					checker = instruction;
					checker = checker >> 16;
					checker = checker & 31;
					output[checker]++;
					total++;
					itype[checker]++;
					checker = instruction;
					checker = checker >> 21;
					checker = checker & 31;
					output[checker]++;
					total++;
					itype[checker]++;
				}
			}
		}
	}

	if(iflag == 1 && uflag == 1)
		printf("TYPE        COUNT    PERCENT\n");

	if(rflag == 1 && uflag == 1)
		printf("REG   USE     R-TYPE  I-TYPE  J-TYPE     PERCENT\n");

	if(iflag == 1) {
		printf("R-type%6s%-8d%1s%.1f%%\n", "", output[0], "", ((float)output[0]/(float)total)*100);
		printf("I-type%6s%-8d%1s%.1f%%\n", "", output[1], "", ((float)output[1]/(float)total)*100);
		printf("J-type%6s%-8d%1s%.1f%%\n", "", output[2], "", ((float)output[2]/(float)total)*100);
	}

	if(rflag == 1) {
		for(i = 0; i <= 31; i++) {
		printf("$%-5d%-7d%1s%-7d%1s%-7d%1s%-7d%4s%.1f%%\n", i, output[i], "", rtype[i], "", itype[i], "", jtype[i], "", ((float)output[i]/(float)total)*100);
		}
	}
	return EXIT_SUCCESS;
}

/*This function prints the menu */
void printmenu(char *const *argv) {
	char *indent = "";
	printf("Usage: %s [-himru]\n", argv[0]);
	printf("%4s-h%9sDisplays this help menu.\n", indent, indent);
	printf("%4s-i%9sDisplays statistics about instruction type usages.\n", indent, indent);
	printf("%4s-m%9sDisplays all the immediate values used in I-Type and J-Type instructions.\n", indent, indent);
	printf("%4s-r%9sDisplays information about the registers.\n", indent, indent);
	printf("%4s-u%9sDisplays human readable headers for the different information displayed.\n %14sShould only be used with the -i, -r, and -m flags.\n\n", indent, indent, indent);
}