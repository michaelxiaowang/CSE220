# Homework #2
# name: MICHAEL_WANG
# sbuid: 109055666

.text
.globl main

main:

#print the menu
menu:
la $a0, menuDisplay
li $v0, 4
syscall

#load menu options into registers to compare for branching
li $t1, 1
li $t2, 2
li $t3, 3

#get input from user
li $v0, 5
syscall
move $s0, $v0 #move input into $s0

#formatting purposes
la $a0, newLineChar
li $v0, 4
syscall

beq $s0, $t3, done #if 3 is selected, terminate program
beq $s0, $t1, getFile #Continue program, will branch based on choice 1 or 2 later
beq $s0, $t2, getFile #Continue program, will branch based on choice 1 or 2 later

#This section of code is reached when neither 1, 2, or 3 are selected. Inform user input is invalid and prompt again
la $a0, badChoice
li $v0, 4
syscall
j menu

############################################################################################################################################
getFile:
#Prompt user for name of text file to read
la $a0, filePrompt
li $v0, 4
syscall

#Reads in the string into strBuffer
la $a0, inputBuffer
li $a1, 256
li $v0, 8
syscall

#Remove the \n at end of file name
li $t9, 0 #create the counter
newCharPtr: lb $t1, inputBuffer($t9) #Sets $t1 to the byte
lb $t2, newLineChar #makes $t2 contain '\n'
beq $t1, $t2, setToZero #check if current byte is equal to '\n'
addi $t9, $t9, 1 #increase counter
j newCharPtr

setToZero:
sb $zero, inputBuffer($t9) #changes the '\n' into \0

#Attempt to open the file
la $a0, inputBuffer
li $a1, 0
li $v0, 13
syscall

#Check if path was valid
move $t0, $v0 #move file descriptor
bgez $t0, validPath

#Inform user file was not opened
la $a0, badPath
li $v0, 4
syscall
j getFile #Jump to getFile

validPath:
move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read first character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

#read first byte to see if it contains UTF-8 byte order mark
move $t3, $a1
lbu $t1, 0($t3)
lb $t2, byteCode
andi $t2, $t2, 0x000000ef
beq $t1, $t2, outfile

#If not UTF-8 encoded, close the file.
la $a0, not8b
li $v0, 4
syscall

j close #close file

##########################################################################################################################
move $s7, $a1

outfile:

#Get output file
la $a0, newFile
li $v0, 4
syscall

#Reads in the string into strBuffer
la $a0, outputBuffer
li $a1, 256
li $v0, 8
syscall

#Remove the \n at end of file name
li $t9, 0 #create the counter
newCharPtr2: lb $t1, outputBuffer($t9) #Sets $t1 to the byte
lb $t2, newLineChar #makes $t2 contain '\n'
beq $t1, $t2, setToZero2 #check if current byte is equal to '\n'
addi $t9, $t9, 1 #increase counter
j newCharPtr2

setToZero2:
sb $zero, outputBuffer($t9) #changes the '\n' into \0

#Attempt to open the file
la $a0, outputBuffer
li $a1, 1
li $v0, 13
syscall

#Check if path was valid
move $t8, $v0
bgez $v0, addBOM

#Inform user file was not opened
la $a0, badPath
li $v0, 4
syscall
j outfile #Jump to outFile

########################################################################################################################

#Adds Little endian BOM to file
addBOM:
li $t1, 2
beq $s0, $t1, BEBOM
li $t1, 0xFEFF
move $a0, $t8
sw $t1, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall

#Adds big endian BOM to file
BEBOM:
li $t1, 0xFFFE
move $a0, $t8
sw $t1, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 2      	    # Finish reading byte mark
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

#formatting purposes
la $a0, newLineChar
li $v0, 4
syscall

while:
#Load bytes one at a time
move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load byte unsigned
move $t9, $v0 #move number of digits read to $t9
beqz $t9, fprint #if number of digits read is 0, terminate program

#If 1 byte based on first few digits, branch to 1 byte procedure
li $t2, 0
srl $t3, $t1, 7
beq $t3, $t2, bytes1

#If 4 bytes based on first few digits, branch to 4 byte procedure
li $t2, 0xf0
andi $t3, $t1, 0xf0
beq $t3, $t2, bytes4

#If 3 bytes based on first few digits, branch to 3 byte procedure
li $t2, 0xe0
andi $t3, $t1, 0xe0
beq $t3, $t2, bytes3

#If 2 bytes based on first few digits, branch to 2 byte procedure
li $t2, 0xc0
andi $t3, $t1, 0xc0
beq $t3, $t2, bytes2

#If neither of the above were the case, inform user it is not a valid UTF8 formatted file
la $a0, not8b
li $v0, 4
syscall

j close #close the file

#####################################################################################################################

bytes1:
#Prints that this is a 1 byte character
la $a0, has1b
li $v0, 4
syscall

#Prints that binary value
la $a0, ($t1)
li $v0, 34
syscall

#Print a space
la $a0, space
li $v0, 4
syscall

#Print code point
la $a0, UTF
li $v0, 4
syscall
la $a0, ($t1)
li $v0, 34
syscall

move $t2, $t1 #we put all the codepoints in t2

#Formatting purposes
la $a0, newLineChar
li $v0, 4
syscall

li $t7, 0x00000#shows not 4 bytes

j encode #Go to encode label

#####################################################################################################################

bytes2:
#Prints that this is a 2 byte character
la $a0, has2b
li $v0, 4
syscall

#Prints first binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t2, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

#Read next byte
move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load second byte
move $t3, $t1

#Print second byte binary
la $a0, ($t1)
li $v0, 34
syscall

#Shift first bit
andi $t2, $t2, 0x1f #Removes the number of bytes indicator
andi $t3, $t3, 0x3f #Removes the number of bytes indicator
sll $t2, $t2, 6 #Shifts first byte over 6
or $t2, $t2, $t3  #Combines the bytes to get code point

#Print a space
la $a0, space
li $v0, 4
syscall

#Print code point
la $a0, UTF
li $v0, 4
syscall
la $a0, ($t2)
li $v0, 34
syscall

#Print a new line
la $a0, newLineChar
li $v0, 4
syscall

li $t7, 0x00000#shows not 4 bytes

j encode #Go to encode label

#####################################################################################################################

bytes3:
#Prints that this is a 3 byte character
la $a0, has3b
li $v0, 4
syscall

#Prints first binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t2, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1          # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load second byte

#Prints second binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t3, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load third byte

#Prints third binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t4, $t1

#Shift first bit
andi $t2, $t2, 0x0f #Removes the number of bytes indicator
sll $t2, $t2, 12 #Shifts first byte over 12

#Shift second bit
andi $t3, $t3, 0x3f #Removes the number of bytes indicator
sll $t3, $t3, 6 #Shifts first byte over 6

#Mask third bit
andi $t4, $t4, 0x3f

#Combines the bytes to get code point
or $t2, $t2, $t3
or $t2, $t2, $t4

#Print a space
la $a0, space
li $v0, 4
syscall

#Print code point
la $a0, UTF
li $v0, 4
syscall
la $a0, ($t2)
li $v0, 34
syscall

#Print a new line
la $a0, newLineChar
li $v0, 4
syscall

li $t7, 0x00000#shows not 4 bytes

j encode #Go to encode label

#####################################################################################################################

bytes4:
la $a0, has4b
li $v0, 4
syscall

#Prints first binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t2, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1          # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load second byte

#Prints second binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t3, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load third byte

#Prints third binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t4, $t1

#Print a space
la $a0, space
li $v0, 4
syscall

move $a0, $t0
la $a1, fileBuffer  # Load the address of the buffer argument
li $a2, 1           # Read a maximum of 1 character
li $v0, 14          # Syscall to read from a file
syscall             # v0 contains the number of characters read

lbu $t1, 0($a1) #load fourth byte

#Prints fourth binary value
la $a0, ($t1)
li $v0, 34
syscall
move $t5, $t1

#Shift first bit
andi $t2, $t2, 0x07 #Removes the number of bytes indicator
sll $t2, $t2, 18 #Shifts first byte over 18

#Shift second bit
andi $t3, $t3, 0x3f #Removes the number of bytes indicator
sll $t3, $t3, 12 #Shifts first byte over 12

#Shift third bit
andi $t4, $t4, 0x3f #Removes the number of bytes indicator
sll $t4, $t4, 6 #Shifts first byte over 6

#Mask fourth bit
andi $t5, $t5, 0x3f

#Combines the bytes to get code point
or $t2, $t2, $t3
or $t2, $t2, $t4
or $t2, $t2, $t5

#Print a space
la $a0, space
li $v0, 4
syscall

#Print code point
la $a0, UTF
li $v0, 4
syscall
la $a0, ($t2)
li $v0, 34
syscall

#Print a new line
la $a0, newLineChar
li $v0, 4
syscall

#This will get surrogate pairs if code point is greater than 0x10000
li $t1, 0x10000
blt $t2, $t1, encode
addi $t2, $t2, -0x10000 #$t2 becomes v'
srl $t3, $t2, 10 #$t3 becomes vh
andi $t4, $t2, 0x3FF #t4 becomes vl
li $t5, 0xD800
li $t6, 0xDC00
li $t7, 0x10000
ori $t2, $t3, 0xD800 #$t2 becomes w1
ori $t3, $t4, 0xDC00 #t3 becomes w2
#####################################################################################################################

encode:
li $t1, 2
li $t4, 0x10000
beq $s0, $t1, utf16BE #If choice 1 selected, do code below for LE encoding, else just to UTF-16BE label
move $a0, $t8
bne $t7, $t4, not4byte
#print first surrogate pair
sw $t2, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall
#print second surrogate pair
sw $t3, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall
j while

utf16BE:
move $a0, $t8
bne $t7, $t4, not4byte2
#print first surrogate pair
srl $t1, $t2, 8
sll $t2, $t2, 8
or $t2, $t2, $t1

sw $t2, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall
#print second surrogate pair
srl $t1, $t3, 8
sll $t3, $t3, 8
or $t3, $t3, $t1

sw $t3, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall
j while

not4byte2:
srl $t1, $t2, 8
sll $t2, $t2, 8
or $t2, $t2, $t1

not4byte:
sw $t2, fileBuffer
la $a1, fileBuffer
li $a2, 2
li $v0, 15
syscall

j while

#Terminates program
done:
li $v0, 10
syscall

#Close the file
close:
move $a0, $t0
li $v0, 16
syscall
#formatting...
la $a0, newLineChar
li $v0, 4
syscall
j menu

#Inform user the encoding was successful
fprint:
la $a0, finish1
li $v0, 4
syscall

la $a0, inputBuffer
li $v0, 4
syscall

li $t1, 2
beq $s0, $t1, finBE #If choice 1 selected, do code below for LE encoding, else just to UTF-16BE label
la $a0, finishLE
li $v0, 4
syscall
j closeOutput

finBE:
la $a0, finishBE
li $v0, 4
syscall
j closeOutput

closeOutput:
move $a0, $t8 #move file descriptor
li $v0, 16
syscall
j close

.data
byteCode: .word 239
inputBuffer: .space 256
outputBuffer: .space 256
writeBuffer: .space 20
fileBuffer: .space 20
filePrompt: .asciiz "Please enter the path to the input file: "
newFile: .asciiz "Please enter a path for a new file: "
badPath: .asciiz "You entered an invalid file path. \n"
badChoice: .asciiz "Choice was invalid. Please enter 1, 2, or 3. \n"
not8b: .asciiz "Not a valid UTF-8 encoded file. Returning to menu... \n\n"
newLineChar: .asciiz "\n"
menuDisplay: .asciiz "=== UTF Converter === \n\n 1. Encode UTF-8 -> UTF16-LE \n 2. Encode UTF-8 -> UTF16-BE \n 3. Exit \n > "
has1b: .asciiz "1 byte "
has2b: .asciiz "2 byte "
has3b: .asciiz "3 byte "
has4b: .asciiz "4 byte "
UTF: .asciiz "U+"
space: .asciiz " "
finish1: .asciiz "\nFile "
finishLE: .asciiz " was successfully encoded in UTF-16LE.\n"
finishBE: .asciiz " was successfully encoded in UTF-16BE.\n"

