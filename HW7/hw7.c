#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "hw7.h"

#ifdef CSE220
	#define subciph(shamt, inputfname, outputfname, ciphertype, cipherop) printf("CSE220: shift amount: %lu\nCSE220: input file: %s\nCSE220: output file: %s\nCSE220: cipher type: %s\nCSE220: cipher operation: %s\n", shamt, inputfname, outputfname, ciphertype, cipherop)
	#define autociph(shamt, inputfname, outputfname, ciphertype, cipherop, tabula) printf("CSE220: shift amount: %lu\nCSE220: input file: %s\nCSE220: output file: %s\nCSE220: cipher type: %s\nCSE220: cipher operation: %s\nCSE220: Tabula Recta\n\n", shamt, inputfname, outputfname, ciphertype, cipherop); \
	int i = 0; \
	do { \
		int j = 0; \
		do { \
			printf("%c ", *(*(tabula+i)+j)); \
			j++; \
		} while (j < ALPHABET_SIZE); \
		printf("\n"); \
		i++; \
	} while (i < ALPHABET_SIZE); \
	printf("\n");
#else
	#define subciph(shamt, inputfname, outputfname, ciphertype, cipherop)
	#define autociph(shamt, inputfname, outputfname, ciphertype, cipherop, tabula)
#endif

void printmenu(char *const *argv);
void uppercaseify(char *text);
void maketabula(int n);
void padkey(char *text, char *oldkey);
void padkeyd(char *text, char *oldkey);

int main(int argc, char **argv) {

	int opt = 0, sflag = 0, aflag = 0, eflag = 0, dflag = 0;
	FILE *inputf;
	FILE *outputf;
	FILE *keyf;
	long unsigned int shamt;
	int i, tabrow, tabcol;
	char *error;

	/*Manages command line flags*/
	while((opt=getopt(argc, argv, "sahed")) != -1) {
		switch(opt) {
			case 'h':
				printmenu(argv);
				return EXIT_SUCCESS;
			case 's':
				sflag = 1;
				break;
			case 'a':
				aflag = 1;
				break;
			case 'e':
				eflag = 1;
				break;
			case 'd':
				dflag = 1;
				break;
			default:
				return EXIT_FAILURE;
		}
	}
	
	/*This section will check the flags for invalid combinations*/
	if(sflag == 1 && aflag == 1)
		return EXIT_FAILURE;
	if(sflag == 1 && (eflag == dflag))
		return EXIT_FAILURE;
	if(aflag == 1 && (eflag == dflag))
		return EXIT_FAILURE;
	if(sflag == 0 && aflag == 0 && eflag == 0 && dflag == 0) {
		printmenu(argv);
		return EXIT_FAILURE;
	}

	/*Set shift amount*/
	shamt = strtol(*(argv+optind), &error, 10);
	if(*error != '\0') {
		return EXIT_FAILURE;
	}	
	shamt = shamt % ALPHABET_SIZE;

	/*Checks to see that valid arguments are found when -s flag provided*/
	if(sflag == 1) {

		/*Checks that there are the right number of positional arguments*/
		if(!(optind < argc && (argc - optind) == 3)) {
			printf("USAGE: %s [-e | -d] n INPUT_FILE OUTPUT_FILE", *argv);
			return EXIT_FAILURE;
		}

		/*Sets the input file*/
		if(strcmp(*(argv + optind + 1), "-") != 0) {
			inputf = fopen(*(argv + optind + 1), "r");
			if(inputf == NULL) {
				return EXIT_FAILURE;
			}
		}
		else {
			inputf = stdin;
		}


		/*Sets the output file*/
		if(strcmp(*(argv + optind + 2), "-") != 0) {
			outputf = fopen(*(argv + optind + 2), "w");
			if(outputf == NULL) {
				return EXIT_FAILURE;
			}
		}
		else {
			outputf = stdout;
		}

		fread(plaintext, BUFFER_SIZE-1, 1, inputf);
		uppercaseify(plaintext);

		if(eflag == 1) {

			/*Perform the encryption*/
			for(i = 0; i < strlen(plaintext); i++) {
				if(*(plaintext + i) >= 65 && *(plaintext + i) <= 90) {
					if(*(plaintext+i) + shamt <= 90) {
						*(ciphertext + i) = *(plaintext + i) + shamt;
					}
					else {
						*(ciphertext + i) = *(plaintext + i) - (ALPHABET_SIZE - shamt);
					}
				}
				else {
					*(ciphertext + i) = *(plaintext + i);
				}
			}
		}

		if(dflag == 1) {

			/*Perform the decryption*/
			for(i = 0; i < strlen(plaintext); i++) {
				if(*(plaintext + i) >= 65 && *(plaintext + i) <= 90) {
					if(*(plaintext+i) - shamt >= 65) {
						*(ciphertext + i) = *(plaintext + i) - shamt;
					}
					else {
						*(ciphertext + i) = *(plaintext + i) + (ALPHABET_SIZE - shamt);
					}
				}
				else {
					*(ciphertext + i) = *(plaintext + i);
				}
			}
		}
	}

	/*Checks to see that valid arguments are found when -a flag provided*/
	if(aflag == 1) {

		/*Checks that there are the right number of positional arguments*/
		if(!(optind < argc && (argc - optind) == 4)) {
			printf("USAGE: %s [-e | -d] n INPUT_FILE KEY_FILE OUTPUT_FILE", *argv);
			return EXIT_FAILURE;
		}

		/*Sets the input file*/
		if(strcmp(*(argv + optind + 1), "-") != 0) {
			inputf = fopen(*(argv + optind + 1), "r");
			if(inputf == NULL) {
				return EXIT_FAILURE;
			}
		}
		else {
			inputf = stdin;
		}

		/*Sets the key file*/
		if(strcmp(*(argv + optind + 2), "-") != 0) {
			keyf = fopen(*(argv + optind + 2), "r");
			if(keyf == NULL) {
				return EXIT_FAILURE;
			}
		}
		else {
			keyf = stdin;
		}

		/*Sets the output file*/
		if(strcmp(*(argv + optind + 3), "-") != 0) {
			outputf = fopen(*(argv + optind + 3), "w");
			if(outputf == NULL) {
				return EXIT_FAILURE;
			}
		}
		else {
			outputf = stdout;
		}

		/*Initialize tabula, get input and key*/
		maketabula(shamt);
		fread(plaintext, BUFFER_SIZE-1, 1, inputf);
		fread(key, KEY_SIZE-1, 1, keyf);
		uppercaseify(plaintext);
		uppercaseify(key);

		/*Perform the encryption*/
		if(eflag == 1) {

			padkey(plaintext, key);
			for(i = 0; i < strlen(plaintext); i++) {
				if(*(plaintext + i) >= 65 && *(plaintext + i) <= 90) {
					tabrow = 0;
					tabcol = 0;
					while(*((*tabula)+tabcol) != *(plaintext + i)) {
						tabcol++;
					}
					while(**(tabula+tabrow) != *(key + i)) {
						tabrow++;
					}
					*(ciphertext + i) = *(*(tabula+tabrow)+tabcol);
				}
				else {
					*(ciphertext + i) = *(plaintext + i);
				}
			}
		}

		/*Perform the decryption*/
		if(dflag == 1) {

			int pad = 0;
			padkeyd(plaintext, key);
			for(i = 0; i < strlen(plaintext); i++) {
				if(*(plaintext + i) >= 65 && *(plaintext + i) <= 90) {
					tabrow = 0;
					tabcol = 0;
					if(i >= strlen(key)) {
						while(*(ciphertext+pad) < 65 || *(ciphertext+pad) > 90) {
							pad++;
						}
						*(key+i) = *(ciphertext+pad);
						pad++;
					}
					while(**(tabula+tabrow) != *(key + i)) {
						tabrow++;
					}
					
					while(*(*(tabula+tabrow)+tabcol) != *(plaintext + i)) {
						tabcol++;
					}
					*(ciphertext + i) = *(*(tabula)+tabcol);
				}
				else {
					*(ciphertext + i) = *(plaintext + i);
				}
			}
		}
	}

	/*DCSE220*/
	if(sflag == 1) {
		if(eflag == 1) {
			if(strcmp(*(argv + optind + 1), "-") != 0) {
				if(strcmp(*(argv + optind + 2), "-") != 0)
					subciph(shamt, *(argv + optind + 1), *(argv + optind + 2), "substitution", "encryption");
				else
					subciph(shamt, *(argv + optind + 1), "STD_OUT", "substitution", "encryption");
			}
			else {
				if(strcmp(*(argv + optind + 2), "-") != 0)
					subciph(shamt, "STD_IN", *(argv + optind + 2), "substitution", "encryption");
				else
					subciph(shamt, "STD_IN", "STD_OUT", "substitution", "encryption");
			}
		}
		else {
			if(strcmp(*(argv + optind + 1), "-") != 0) {
				if(strcmp(*(argv + optind + 2), "-") != 0)
					subciph(shamt, *(argv + optind + 1), *(argv + optind + 2), "substitution", "decryption");
				else
					subciph(shamt, *(argv + optind + 1), "STD_OUT", "substitution", "decryption");
			}
			else {
				if(strcmp(*(argv + optind + 2), "-") != 0)
					subciph(shamt, "STD_IN", *(argv + optind + 2), "substitution", "decryption");
				else
					subciph(shamt, "STD_IN", "STD_OUT", "substitution", "decryption");
			}
		}
	}
	else {
		if(eflag == 1) {
			if(strcmp(*(argv + optind + 1), "-") != 0) {
				if(strcmp(*(argv + optind + 3), "-") != 0) {
					autociph(shamt, *(argv + optind + 1), *(argv + optind + 3), "autokey", "encryption", tabula);
				}
				else {
					autociph(shamt, *(argv + optind + 1), "STD_OUT", "autokey", "encryption", tabula);
				}
			}
			else {
				if(strcmp(*(argv + optind + 3), "-") != 0) {
					autociph(shamt, "STD_IN", *(argv + optind + 3), "autokey", "encryption", tabula);
				}
				else {
					autociph(shamt, "STD_IN", "STD_OUT", "autokey", "encryption", tabula);
				}
			}
		}
		else {
			if(strcmp(*(argv + optind + 1), "-") != 0) {
				if(strcmp(*(argv + optind + 3), "-") != 0) {
					autociph(shamt, *(argv + optind + 1), *(argv + optind + 3), "autokey", "decryption", tabula);
				}
				else {
					autociph(shamt, *(argv + optind + 1), "STD_OUT", "autokey", "decryption", tabula);
				}
			}
			else {
				if(strcmp(*(argv + optind + 3), "-") != 0) {
					autociph(shamt, "STD_IN", *(argv + optind + 3), "autokey", "decryption", tabula);
				}
				else {
					autociph(shamt, "STD_IN", "STD_OUT", "autokey", "decryption", tabula);
				}
			}
		}
	}

	fprintf(outputf, ciphertext);
	if(outputf != stdout)
		fclose(outputf);

	return EXIT_SUCCESS;
}

/*This function prints the menu*/
void printmenu(char *const *argv) {
	printf("Usage: %s [-s | -a | -h]\n", *argv);
	printf("%4s-s%7sSubstitution cipher\n", "", "");
	printf("%13sAdditional parameters: [-e | -d] n INPUT_FILE OUTPUT_FILE\n", "");
	printf("%17s-e Encrypt using the substitution cipher.\n", "");
	printf("%17s-d Decrypt using the substitution cipher.\n", "");
	printf("%18sn The amount of position to shift by.\n", "");
	printf("%18sINPUT_FILE This can be any file on the file system or - which specifies stdin.\n", "");
	printf("%18sOUTPUT_FILE This can be any file on the system or - which specifies stdout.\n", "");
	printf("%4s-s%7sAutokey cipher\n", "", "");
	printf("%13sAdditional parameters: [-e | -d] n INPUT_FILE KEY_FILE OUTPUT_FILE\n", "");
	printf("%17s-e Encrypt using the autokey cipher.\n", "");
	printf("%17s-d Decrypt using the autokey cipher.\n", "");
	printf("%18sn The initial shift value.\n", "");
	printf("%18sINPUT_FILE This can be any file on the file system or - which specifies stdin.\n", "");
	printf("%18sKEY_FILE This can be any file on the file system or - which specifies stdin.\n", "");
	printf("%18sOUTPUT_FILE This can be any file on the system or - which specifies stdout.\n", "");
	printf("%4s-h%7sDisplay this help menu.\n", "", "");
}

/*This function makes all the characters upper case*/
void uppercaseify(char *text) {
	int i;
	int textint;
	for(i = 0; i < strlen(text); i++) {
		textint = *(text + i);
		*(text + i) = toupper(textint);
	}
}

/*This functions makes our tabula recta*/
void maketabula(int n) {
	int i, j;

	/*The first element of the tabula is A shifted by n*/
	**tabula = 65 + n;

	/*Every element in the first row is the previous element shifted by 1*/
	for(i = 1; i < ALPHABET_SIZE; i++) {
		if(*(*tabula+(i-1)) == 90) {
			*(*tabula+i) = 65;
		}
		else {
			*(*tabula+i) = *(*tabula+(i-1)) + 1;
		}
		/*Set the first element of the ith row to the ith column*/
		**(tabula + i) = *(*tabula+i);
	}
	/*Every element in the ith row is the previous element shifted by 1*/
	for(i = 1; i < ALPHABET_SIZE; i++) {
		for(j = 1; j < ALPHABET_SIZE; j++) {
			if(*(*(tabula+i)+(j-1)) == 90) {
				*(*(tabula+i)+j) = 65;
			}
			else {
				*(*(tabula+i)+j) = *(*(tabula+i)+(j-1)) + 1;
			}
		}
	}
}

/*This function will pad the key*/
void padkey(char *text, char *oldkey) {
	int i;
	strcpy(buffer, oldkey);
	int lastchr = 0, lastchr2 = 0;
	for(i = 0; i < strlen(text); i++) {
		if(*(text + i) >= 65 && *(text + i) <= 90) {
			if(lastchr < strlen(buffer)) {
				while(!(*(buffer + lastchr) >= 65 && *(buffer + lastchr) <= 90) && lastchr < strlen(buffer)) {
					lastchr++;
				}
				*(oldkey + i) = *(buffer + lastchr);
				lastchr++;
			}
			else {
				while(!(*(text + lastchr2) >= 65 && *(text + lastchr2) <= 90)) {
					lastchr2++;
				}
				*(oldkey + i) = *(text + lastchr2);
				lastchr2++;
			}
		}
		else {
			*(oldkey + i) = ' ';
		}
	}
}

/*This function will pad the key for decryption*/
void padkeyd(char *text, char *oldkey) {
	int i;
	strcpy(buffer, oldkey);
	int lastchr = 0;
	for(i = 0; i < strlen(text); i++) {
		if(*(text + i) >= 65 && *(text + i) <= 90) {
			while(!(*(buffer + lastchr) >= 65 && *(buffer + lastchr) <= 90) && lastchr < strlen(buffer)) {
				lastchr++;
			}
			*(oldkey + i) = *(buffer + lastchr);
			lastchr++;
		}
		else {
			*(oldkey + i) = ' ';
		}
	}
}
