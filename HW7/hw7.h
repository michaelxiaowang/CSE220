#ifndef HW7_H
#define HW7_H

#define BUFFER_SIZE 256
#define ALPHABET_SIZE 26
#define KEY_SIZE 256

/**
 * Can use this array to store your plaintext
 * messages for encryption/decryption.
 */
char plaintext[BUFFER_SIZE];

/**
 * Can use this array to store your ciphertext
 * for encryption.decryption.
 */
char ciphertext[BUFFER_SIZE];

/**
 * Can use this array to store your key that is
 * used for encryption.decryption.
 */
char key[KEY_SIZE];

/**
 * Should store your alphabet for the substitution cipher.
 */
char alphabet[ALPHABET_SIZE];

/**
 * Should store your tabula recta for the autokey cipher.
 */
char tabula[ALPHABET_SIZE][ALPHABET_SIZE];

/**
 * Additional place you might want to use. Don't have too.
 * You can use some casting magic to change type from char 
 * if you need too.
 */
char buffer[BUFFER_SIZE];

#endif