/*
Homework #9
name: MICHAEL_WANG
sbuid: 109055666
EXTRA_CREDIT_COMPLETE
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hw9.h"

#ifdef CSE220
	#define plist(itype) printlist(rtype)
	#define pdata(itype) printdata(rtype, totalmem)
#else
	#define plist(itype)
	#define pdata(itype)
#endif

/*Function headers*/
void printmenu(char *const *argv);
void freeTypes(InstrType *type1, InstrType *type2, InstrType *type3);
int addNode(InstrType *type, Instr *node);
void printlist(InstrType *type);
void printdata(InstrType *type, unsigned int totalmem);
void changeEndianness(unsigned int *value);
unsigned int getOpcode(unsigned int *value);
unsigned int getUid(unsigned int *value, unsigned int *opcode);
int printInstruction(InstrType *type, unsigned int *value, unsigned int *opcode, FILE *outputf);
char *getRegisterHuman(unsigned int *registerNum);


int main(int argc, char *argv[]) {
	int opt;
	char *mapfname = NULL;
	char *inputfname = NULL;
	char *outputfname = NULL;

	/*First make the instruction types. In any case, if malloc fails, free the other malloc'd memory*/
	/*Manages command line flags*/
	while((opt=getopt(argc, argv, "m:i:o:h")) != -1) {
		switch(opt) {
			case 'h':
				printmenu(argv);
				return EXIT_SUCCESS;
			case 'm':
				mapfname = optarg;
				break;
			case 'i':
				inputfname = optarg;
				break;
			case 'o':
				outputfname = optarg;
				break;
			default:
				return EXIT_FAILURE;
		}
	}

	if(inputfname == NULL || outputfname == NULL) {
		printf("There was an error with the arguments.\n");
		printmenu(argv);
		return EXIT_FAILURE;
	}

	if(mapfname != NULL) {
		if(*inputfname == *mapfname || *outputfname == *mapfname) {
			fprintf(stderr, "One or more of the files have the same name.");
			return EXIT_FAILURE;
		}
	}

	if(*inputfname == *outputfname) {
		fprintf(stderr, "One or more of the files have the same name.");
		return EXIT_FAILURE;
	}

	FILE *outputf;
	if(*outputfname == '-')
		outputf = stdout;
	else
		outputf = fopen(outputfname, "w");
	if(outputf == NULL) {
		fprintf(stderr, "The file \"%s\" does not exist or could not be opened.", outputfname);
		return EXIT_FAILURE;
	}

	FILE *mapping;
	if(mapfname != NULL)
		mapping = fopen(mapfname, "r");
	else
		mapping = fopen("instruction_mapping.txt", "r");
	if(mapping == NULL) {
		fprintf(stderr, "The file \"%s\" does not exist or could not be opened.", inputfname);
		return EXIT_FAILURE;
	}

	/*Check the endianness of architecture*/
	int endianness = 1; //0 is BE, 1 is LE
	char *endiantest = (char*)&endianness;
	if(*endiantest) {
		endianness = 1;
	}
	else {
		endianness = 0;
	}
	
	unsigned int totalmem = 0;

	InstrType *rtype = malloc(sizeof(InstrType));
	if(rtype == NULL)
		return EXIT_FAILURE;
	rtype->type = 'R';
	rtype->head = NULL;
	rtype->count = 0;

	InstrType *itype = malloc(sizeof(InstrType));
	if(itype == NULL) {
		free(itype);
		return EXIT_FAILURE;
	}
	itype->type = 'I';
	itype->head = NULL;
	itype->count = 0;

	InstrType *jtype = malloc(sizeof(InstrType));
	if(jtype == NULL) {
		free(itype);
		free(rtype);
		return EXIT_FAILURE;
	}
	jtype->type = 'J';
	jtype->head = NULL;
	jtype->count = 0;

	/*Link the instruction types*/
	rtype->prev = NULL;
	rtype->next = itype;
	itype->prev = rtype;
	itype->next = jtype;
	jtype->prev = itype;
	jtype->next = NULL;

	totalmem += sizeof(InstrType) * 3;

	char buffer[256];
	while(fgets(buffer, 255, mapping) != NULL) {
		/*Make a node and initialize it*/
		Instr *node = malloc(sizeof(Instr));
		if(node == NULL) {
			freeTypes(rtype, itype, jtype);
			return EXIT_FAILURE;
		}
		totalmem += sizeof(Instr);
		node->next = NULL;
		node->prev = NULL;
		node->mnemonic = malloc(strlen(buffer) - 14);
		char type;
        int numscanned = sscanf(buffer, "%c %x %s %d", &type, &node->uid, node->mnemonic, &node->pretty);
        if(numscanned != 4) {
        	freeTypes(rtype, itype, jtype);
        	printf("Error scanning the file, the instruction file should have one instruction per line with 4 elements separated by space: type uid mnemonic pretty (i.e. r 00000020 add 3)\n");
        	return EXIT_FAILURE;
        }
        if(node->pretty > 9 || node < 0 || !(type == 'r' || type == 'i' || type == 'j')) {
        	freeTypes(rtype, itype, jtype);
        	printf("There was a problem with the file. Please make sure the file follows these rules: \n 1) type must be 'r', 'i' or 'j' \n 2) pretty value must be 0-9.\n");
        }

        totalmem += strlen(node->mnemonic);

        int addSuccessful = 0;

        /*Add the node to the right list*/
        switch(type) {
        	case 'r':
        		addSuccessful = addNode(rtype, node);
        		break;
        	case 'i':
        		addSuccessful = addNode(itype, node);
        		break;
        	case 'j':
        		addSuccessful = addNode(jtype, node);
        		break;
        }

        if(addSuccessful == 1) {
        	freeTypes(rtype, itype, jtype);
        	printf("Problem adding a node. One or more of the instructions in a list may have the same name.");
        	return EXIT_FAILURE;
        }

	}

	/*If compiled with -DSE220 flag, print out lists*/
	plist(rtype);
	pdata(rtype);
	
		/*Open the binary file*/
	FILE *binary = fopen(inputfname, "rb");
	if(binary == NULL) {
		fprintf(stderr, "The file \"%s\" does not exist or could not be opened.", mapfname);
		return EXIT_FAILURE;
	}

	unsigned int value = 0;
	int BOM; //0 is BE, 1 is LE

	/*Read and evaluate Byte-Order Mark (BOM)*/
	fread(&value, 4, 1, binary);
	if(value != 0x576f6e67 && value != 0x676f6e57 && value != 0x576e6f67 && value != 0x676e6f57) {
		fprintf(stderr, "Invalid BOM");
		return EXIT_FAILURE;
	}
	else {
		if(value == 0x576f6e67) {
			if(endianness == 0) {
				BOM = 0;
			}
			else {
				BOM = 1;
			}
		}
		if(value == 0x676f6e57) {
			if(endianness == 0) {
				BOM = 1;
			}
			else {
				BOM = 0;
			}
		}
		if(value == 0x576e6f67) {
			if(endianness == 0) {
				BOM = 0;
			}
			else {
				BOM = 1;
			}
		}
		if(value == 0x676e6f57) {
			if(endianness == 0) {
				BOM = 1;
			}
			else {
				BOM = 0;
			}
		}
	}

	/*Read the binary file and build instructions*/
	while(fread(&value, 4, 1, binary) != 0) {
		if(BOM != endianness) {
			changeEndianness(&value);
		}
		unsigned int opcode = getOpcode(&value);
		InstrType *type;

		switch(opcode) {
			case 0x0:
				type = rtype;
				break;
			case 0x2:
				type = jtype;
				break;
			case 0x3:
				type = jtype;
				break;
			default:
				type = itype;
				break;
		}

		if(printInstruction(type, &value, &opcode, outputf) == 1) {
			printf("Error printing");
			unlink(outputfname);
			freeTypes(rtype, itype, jtype);
			return EXIT_FAILURE;
		}
	}

	/*If output file was not stdin, close the file*/
	if(outputf != stdout)
		fclose(outputf);

	/*If mapping file was opened, close it*/
	if(mapfname != NULL) {
		fclose(mapping);
	}

	fclose(binary);

	/*Free our memory before terminating program*/
	freeTypes(rtype, itype, jtype);
	return EXIT_SUCCESS;
}

/*Print the menu*/
void printmenu(char *const *argv) {
	printf("Usage: %s [-h] [-m INSTRUCTION_MAPPING] -i INPUT_FILE -o OUTPUT_FILE\n", *argv);
	printf("%-23sDisplays this help menu.\n", "-h");
	printf("%-23sFile that contains the instruction mapping. If this option is not provided it defaults to instruction_mapping.txt.\n", "-m INSTRUCTION_MAPPING");
	printf("%-23sThis should be a binary file that contains the .text section of a Mars220 MIPS binary.\n", "-i INPUT_FILE");
	printf("%-23sThis can be any file on the system or \"-\" which specifies stdout.\n", "-o OUTPUT_FILE");
}

/*When called, will free the memory of the InstrTypes*/
void freeTypes(InstrType *type1, InstrType *type2, InstrType *type3) {

	unsigned int totalmemfreed = 0;

	InstrType *type = type3;

	Instr *nodePtr;

	while(type != NULL) {

		nodePtr = type->head;

		if(nodePtr != NULL) {

			/*Move to the end of the list*/
			while(nodePtr->next != NULL) {
				nodePtr = nodePtr->next;
			}
			/*Remove all the nodes in the list*/
			while(nodePtr != NULL) {
				totalmemfreed += strlen(nodePtr->mnemonic);
				free(nodePtr->mnemonic);
				nodePtr->mnemonic = NULL;
				totalmemfreed += sizeof(Instr);
				free(nodePtr->next);
				nodePtr = nodePtr->prev;
			}
			
		}

		/*Now remove the InstrType node*/
			totalmemfreed += sizeof(InstrType);	
			free(type);
			type = type->prev;
	}
	//printf("MEMORY FREED IS %u", totalmemfreed);
}

/*When called, will add a node the type*/
int addNode(InstrType *type, Instr *node) {

	/*If the head is NULL, the node is first item in list*/
	if(type->head == NULL) {
		type->head = node;
		type->count++;
		return 0;
	}

	/*Otherwise, if it comes before head in the alphabet*/
	/*Then set the InstrType head to this node*/
	else if(strcmp(node->mnemonic, type->head->mnemonic) <= 0) {
		if(strcmp(node->mnemonic, type->head->mnemonic) == 0) {
			return 1;
		}
		node->next = type->head;
		type->head->prev = node;
		type->head = node;
		type->count++;
		return 0;
	}

	/*If it is not before the head in the alphabet*/
	/*Then find its rightful spot*/
	else {
		Instr *nodeptr = type->head;
		while(nodeptr->next != NULL) {
			if(strcmp(node->mnemonic, nodeptr->next->mnemonic) == 0) {
				return 1;
			}
			else if(strcmp(node->mnemonic, nodeptr->next->mnemonic) < 0) {
				node->next = nodeptr->next;
				node->prev = nodeptr;
				node->next->prev = node;
				node->prev->next = node;
				type->count++;
				return 0;
			}
			nodeptr = nodeptr->next;
		}
		/*If it reached end of loop, then node comes last alphabetically*/
		nodeptr->next = node;
		node->prev = nodeptr;
		type->count++;
		return 0;
	}
}

/*This will print out the lists of instructions*/
void printlist(InstrType *type) {
	InstrType *instrType = type;
	Instr *nodePtr;
	unsigned int nodePtrID;
	while(instrType != NULL) { 
		printf("CSE220: %c-Type List:\n", instrType->type); 
		nodePtr = instrType->head; 
		while(nodePtr != NULL) { 
			if(getOpcode(&(nodePtr->uid)) != 0) {
        		nodePtrID = nodePtr->uid >> 26;
        		nodePtrID = nodePtrID & 0x3f;
    		}
    		else {
    			nodePtrID = nodePtr->uid;
    		}
				printf("CSE220: %-11p uid: %-5d pretty: %-u mnemonic: %-7s next: %-11p prev: %-11p\n", nodePtr, nodePtrID, nodePtr->pretty, nodePtr->mnemonic, nodePtr->next, nodePtr->prev); 
			nodePtr = nodePtr->next;
		} 
		instrType = instrType->next;
	}
	printf("\n");
}

/*This will print out data about memory usage*/
void printdata(InstrType *type, unsigned int totalmem) {
	InstrType *instrType = type;
	unsigned int rtypenodes = type->count;
	type = type->next;
	unsigned int itypenodes = type->count;
	type = type->next;
	unsigned int jtypenodes = type->count;
	unsigned int totalnodes = itypenodes + rtypenodes + jtypenodes;
	totalnodes = totalnodes + 3;
	unsigned int instrtypesize = sizeof(InstrType);
	unsigned int instrsize = sizeof(Instr);
	Instr *nodePtr;
	type = instrType;
	while(instrType != NULL) { 
		nodePtr = instrType->head;
		while(nodePtr != NULL) {
			nodePtr = nodePtr->next;
		} 
		instrType = instrType->next;
	}
	printf("CSE220: There is %u r-type, %u i-type, and %u j-type nodes.\n", rtypenodes, itypenodes, jtypenodes);
	printf("CSE220: The InstrType node takes up %u bytes in memory.\n", instrtypesize);
	printf("CSE220: The Instr node takes up %u bytes in memory.\n", instrsize);
	printf("CSE220: Your program allocated %u nodes that takes up %u bytes in memory.\n", totalnodes, totalmem);
	printf("\n");
}

/*This will change the endianness of a 4 byte value*/
void changeEndianness(unsigned int *value) {
	unsigned int first = *value << 24;

	unsigned int second = *value << 16;
	second = second >> 8;

	unsigned int third = *value << 8;
	third = third >> 16;

	unsigned int fourth = *value;
	fourth = fourth >> 24;

	first = first & 0xff000000;
	second = second & 0x00ff0000;
	third = third & 0x0000ff00;
	fourth = fourth & 0x000000ff;

	*value = first | second | third | fourth;
}

/*This will get out opcode*/
unsigned int getOpcode(unsigned int *value) {
	unsigned int opcode = *value >> 26;
	opcode = opcode & 0x0000003f;
	return opcode;
}

/*This will get the uid*/
unsigned int getUid(unsigned int *value, unsigned int *opcode) {

	unsigned int uid;

	switch(*opcode) {
		case 0x0:
			uid = *value & 0x0000003f;
			break;
		default:
			uid = *value & 0xfc000000;
			break;
	}

	return uid;
}

/*This will get the node*/
int printInstruction(InstrType *type, unsigned int *value, unsigned int *opcode, FILE *outputf) {

	/*Get the uid*/
	unsigned int uid = getUid(value, opcode);

	/*Start searching in the right list*/
	Instr *nodePtr = type->head;

	/*Get the node in question*/
	while(nodePtr != NULL) {

		if(nodePtr->uid == uid)
			break;

		nodePtr = nodePtr->next;
	}

	/*Get the register and immediate values*/
	unsigned int rs = (*value >> 21) & 0x1f;
	unsigned int rt = (*value >> 16) & 0x1f;
	unsigned int rd = (*value >> 11) & 0x1f;
	unsigned int shamt = (*value >> 6) & 0x1f;
	unsigned int iImmed = *value & 0xffff;
	unsigned int jImmed = *value & 0x3ffffff;
	char *rshum = getRegisterHuman(&rs);
	char *rthum = getRegisterHuman(&rt);
	char *rdhum = getRegisterHuman(&rd);

	char buffer[256];

	/*Print the pretty node*/
	switch(nodePtr->pretty) {
		case LOOK_1_REG:
			sprintf(buffer, "%s $%s\n", nodePtr->mnemonic, rdhum);
			break;
		case LOOK_2_REG:
			sprintf(buffer, "%s $%s, $%s\n", nodePtr->mnemonic, rshum, rthum);
			break;
		case LOOK_2_REG_IMMED:
			sprintf(buffer, "%s $%s, $%s, 0x%x\n", nodePtr->mnemonic, rthum, rshum, iImmed);
			break;
		case LOOK_3_REG:
			sprintf(buffer, "%s $%s, $%s, $%s\n", nodePtr->mnemonic, rdhum, rthum, rshum);
			break;
		case LOOK_MEM:
			sprintf(buffer, "%s $%s 0x%x($%s)\n", nodePtr->mnemonic, rthum, iImmed, rshum);
			break;
		case LOOK_SYSCALL:
			sprintf(buffer, "%s\n", nodePtr->mnemonic);
			break;
		case LOOK_JUMP:
			sprintf(buffer, "%s 0x%x\n", nodePtr->mnemonic, jImmed);
			break;
		case LOOK_1_REG_IMMED:
			sprintf(buffer, "%s $%s, 0x%x\n", nodePtr->mnemonic, rshum, iImmed);
			break;
		case LOOK_SHIFT:
			sprintf(buffer, "%s $%s, $%s, 0x%x\n", nodePtr->mnemonic, rdhum, rshum, shamt);
			break;
		case LOOK_BRANCH:
			sprintf(buffer, "%s $%s, $%s, 0x%x\n", nodePtr->mnemonic, rshum, rthum, iImmed);
			break;		
	}
	if(fprintf(outputf, buffer) != strlen(buffer)) {
		return 1;
	}
	else {
		return 0;
	}
}

/*Take register num value and return register human value*/
char *getRegisterHuman(unsigned int *registerNum) {
	switch(*registerNum) {
		case 0:
			return "zero";
			break;
		case 1:
			return "at";
			break;
		case 2:
			return "v0";
			break;
		case 3:
			return "v1";
			break;
		case 4:
			return "a0";
			break;
		case 5:
			return "a1";
			break;
		case 6:
			return "a2";
			break;
		case 7:
			return "a3";
			break;
		case 8:
			return "t0";
			break;
		case 9:
			return "t1";
			break;
		case 10:
			return "t2";
			break;
		case 11:
			return "t3";
			break;
		case 12:
			return "t4";
			break;
		case 13:
			return "t5";
			break;
		case 14:
			return "t6";
			break;
		case 15:
			return "t7";
			break;
		case 16:
			return "s0";
			break;
		case 17:
			return "s1";
			break;
		case 18:
			return "s2";
			break;
		case 19:
			return "s3";
			break;
		case 20:
			return "s4";
			break;
		case 21:
			return "s5";
			break;
		case 22:
			return "s6";
			break;
		case 23:
			return "s7";
			break;
		case 24:
			return "t8";
			break;
		case 25:
			return "t9";
			break;
		case 26:
			return "k1";
			break;
		case 27:
			return "k2";
			break;
		case 28:
			return "gp";
			break;
		case 29:
			return "fp";
			break;
		case 30:
			return "sp";
			break;
		case 31:
			return "ra";
			break;
		default:
			return NULL;
	}
}