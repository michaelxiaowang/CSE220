#Homework #1
#name: MICHAEL_WANG
#sbuid: 109055666

#The following output is given when inputting the ascii characters AbCd(41624364) but stored in little endian(64436241), which is 1682137665 in decimal:
#Enter an integer number: 1682137665
#2's complement: 1682137665 0x64436241 01100100010000110110001001000001 1682137665
#1's complement: 1682137665 0x64436241 01100100010000110110001001000001 1682137665
#Sign Magnitude: 1682137665 0x64436241 01100100010000110110001001000001 1682137665
#Neg 2's complement: -1682137665 0x9bbc9dbf 10011011101111001001110110111111 -1682137665
#16-bit 2's complement: 25153 0x00006241 00000000000000000110001001000001 25153

.text
.globl main
main:
#prompts user for integer input
la $a0, str
li $v0, 4
syscall

#stores integer received into $v0
li $v0, 5
syscall

la $s0, ($v0) #save integer in $s0

#print 2's complement row
la $a0, twoc
li $v0, 4
syscall #prints the the string for type of representation

la $a0, ($s0)
li $v0, 1
syscall #rep value as integer

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 34
syscall #print hex representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 35
syscall #print binary representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 1
syscall #print integer value of binary

la $a0, enter
li $v0, 4
syscall #make a new line

la $s1, ($s0) #puts integer value into $s1 (1's complement saved binary)
la $s2, ($s0) #put integer value into $s2 (signed magnitude saved binary)

#checks if number is negative, if it is make adjustments for 1's complement and sign magnitude representations
bgez $s1, nonneg #if negative do code below first, or jump to nonneg
addi $s1, $s0, -1 #subtract 1 from $s1
li $t1, -1 #load -1 into $t1
mul $s2, $s2, $t1 #multiply $s2 by -1
ori $s2, $s2, 0x80000000 #add the 1 sign bit to left most bit

#print 1's complement row
nonneg:
la $a0, onec
li $v0, 4
syscall #prints the the string for type of representation

la $a0, ($s0)
li $v0, 1
syscall #rep value as integer

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s1)
li $v0, 34
syscall #print hex representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s1)
li $v0, 35
syscall #print binary representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s1)
li $v0, 1
syscall #print integer value of binary

la $a0, enter
li $v0, 4
syscall #make a new line

#print sign magnitude row
la $a0, sigm
li $v0, 4
syscall #prints the the string for type of representation

la $a0, ($s0)
li $v0, 1
syscall #rep value as integer

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s2)
li $v0, 34
syscall #print hex representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s2)
li $v0, 35
syscall #print binary representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s2)
li $v0, 1
syscall #print integer value of binary

la $a0, enter
li $v0, 4
syscall #make a new line

#print neg 2's complement
li $t1, -1
mul $s3, $s0, $t1
la $a0, neg2
li $v0, 4
syscall #prints the the string for type of representation

la $a0, ($s3)
li $v0, 1
syscall #rep value as integer

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s3)
li $v0, 34
syscall #print hex representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s3)
li $v0, 35
syscall #print binary representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s3)
li $v0, 1
syscall #print integer value of binary

la $a0, enter
li $v0, 4
syscall #make a new line

#print 16-bit 2's complement row
la $a0, stb2
li $v0, 4
sll $s0, $s0, 16 #shift bits 16 units left
srl $s0, $s0, 16 #shift bits 16 units right
syscall #prints the the string for type of representation

la $a0, ($s0)
li $v0, 102
syscall #rep value as integer

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 34
syscall #print hex representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 35
syscall #print binary representation of rep value

la $a0, space
li $v0, 4
syscall #make a space

la $a0, ($s0)
li $v0, 1
syscall #print integer value of binary

la $a0, enter
li $v0, 4
syscall #make a new line

#terminates program
li $v0, 10
syscall

.data
str: .asciiz "Enter an integer number: "
twoc: .asciiz "2's complement: "
onec: .asciiz "1's complement: "
sigm: .asciiz "Sign Magnitude: "
neg2: .asciiz "Neg 2's complement: "
stb2: .asciiz "16-bit 2's complement: "
space: .asciiz " "
enter: "\n"
