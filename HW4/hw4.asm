.text

##########################################################################################################
#                                             Part 1                                                     #
##########################################################################################################

remainder:
	# save the address we need to jump to
	addi $sp, $sp, -4 # make space to store this activation record's return address
	sw $ra, 0($sp) # we need to save our return address

	#################################################################################################
	#                                           base case                                           #
	#################################################################################################
	beqz $a1, getRemainder # if divisor is 0, return the argument
	blt $a0, $a1, getRemainder # base case: if argument < divisor, return the argument
	
	#################################################################################################
	#                                           recursion                                           #
	#################################################################################################
	sub $a0, $a0, $a1 # subtract divisor from argument
	jal remainder # recursively call with new argument

	getRemainder:
	move $v0, $a0 # move the remainder obtained as a result recursion from $a0 to $v0, which is the return register
	lw $ra, 0($sp) # load this activation record's return address
	addi $sp, $sp, 4 # restore the stack pointer
	jr $ra # return to where function was called

printBase:
	# check if base chosen was invalid (base > 9 || base < 2)
	li $t0, 9 # load the value 9 into a temporary register, $t0
	bgt $a1, $t0, invalidBase # if the base value in $a0 > 9, inform user the base is invalid
	li $t0, 2 # load the value 2 into a temporary register, $t0
	blt $a1, $t0, invalidBase # if the base value in $a0 < 2, inform user the base is invalid

	# store the return address of this activation record
	addi $sp, $sp, -4 # make space to store this activation record's return address
	sw $ra, 0($sp) # we need to save our return address
	
	#################################################################################################
	#                                           base case                                           #
	#################################################################################################
	# if $a0 < base, don't recursively call, just get the remainder
	blt $a0, $a1, printBaseRemainder
	
	#################################################################################################
	#                                           recursion                                           #
	#################################################################################################
	# this section is a recursive call to printBase
	addi $sp, $sp, -4 # make space to store this activation record's $a0
	sw $a0, 0($sp) # a0 is used in remainder, so it must be saved
	divu $a0, $a1 # divide $a0 by $a1
	mflo $a0  # put the quotient of $a0 / $a1 in $a0
	jal printBase # call printBase on the quotient of $a0 / $a1
	lw $a0, 0($sp) # get this activation record's $a0 value
	addi $sp, $sp, 4 # restore the stack pointer
	
	# this section gets the remainder
	printBaseRemainder:
	jal remainder # get the remainder; it is in $v0
	move $a0, $v0 # move value from $v0 to $a0
	li $v0, 1 # load the print int syscall value into $v0
	syscall # print the remainder
	
	j getPrintBaseAddress # if code reached here, we had a valid base; we don't print invalid base

	# this section is only run if $a1 < 2 or $a1 > 9
	invalidBase:
	la $a0, printInvalidBase # load the label invalidBase into $a0
	li $v0, 4 # load the print String syscall value into $v0
	syscall # tell user base was invalid
	j dontLoadAddress # return to function caller

	getPrintBaseAddress:
	lw $ra, 0($sp) # load this activation record's return address
	addi $sp, $sp, 4 # restore the stack pointer
	dontLoadAddress: 
	jr $ra # return to where function was called

##########################################################################################################
#                                             Part 2                                                     #
##########################################################################################################

levenshteinDistance:
	addi $sp, $sp, -4 # make space for return address
	sw $ra, 0($sp) # store the return address
	
	#################################################################################################
	#                                           base case                                           #
	#################################################################################################
	beqz $a1, levenshteinReturnLenT # if the lenS is 0, return lenT
	beqz $a3, levenshteinReturnLenS # if the lenT is 0, return lenS
	
	j levenshteinRecursion # base cases not met, call recursion
	
	levenshteinReturnLenT:	
	move $v0, $a3 # move LenT to return value
	j levenshteinReturn
	
	levenshteinReturnLenS:
	move $v0, $a1 # move LenS to return value
	j levenshteinReturn

	#################################################################################################
	#                                           recursion                                           #
	#################################################################################################
	
	#################################################################################################
	#                                           get match                                           #
	#################################################################################################
	levenshteinRecursion:
	addiu $t1, $a1, -1 # this holds LenS - 1
	la $t2, ($a0) # copy string address to $t2
	add $t2, $t1, $t2 # add offset to $t2 and store in $t2
	
	lbu $t3, 0($t2) # load the last character of s into $t3	

	addiu $t1, $a3, -1 # this holds LenT - 1
	la $t2, ($a2) # copy string address to $t2
	add $t2, $t1, $t2 # add offset to $t2 and store in $t2

	lbu $t4, 0($t2) # load the last character of t into $t4
	
	li $t0, 0 # this will hold match value
	beq $t3, $t4, gotMatchValue
	li $t0, 1 # set $t0 to 1 (not matched)
	
	gotMatchValue:	
	#################################################
	# do levenshteinDistance (s, LenS - 1, t, LenT) #
	#################################################
	addi $sp, $sp, -32 # make space for the activation record
	sw $a0, 28($sp) # store $a0
	sw $a1, 24($sp) # store $a1
	sw $a2, 20($sp) # store $a2
	sw $a3, 16($sp) # store $a3
	sw $t0, 12($sp) # store $t0
		
	addiu $a1, $a1, -1 # change $a1 from LenS to LenS - 1
	jal levenshteinDistance
	move $t1, $v0
	addiu $t1, $t1, 1
	sw $t1, 8($sp)
	
	#################################################
	# do levenshteinDistance (s, LenS, t, LenT - 1) #
	#################################################
	lw $a0, 28($sp) # load original $a0 value back into $a0
	lw $a1, 24($sp) # load original $a1 value back into $a1
	lw $a2, 20($sp) # load original $a1 value back into $a2
	lw $a3, 16($sp) # load original $a1 value back into $a3

	addiu $a3, $a3, -1 # change $a3 from LenT to LenT - 1
	jal levenshteinDistance
	move $t2, $v0
	addiu $t2, $t2, 1
	sw $t2, 4($sp)
	
	#####################################################
	# do levenshteinDistance (s, LenS - 1, t, LenT - 1) #
	#####################################################
	lw $a0, 28($sp) # load original $a0 value back into $a0
	lw $a1, 24($sp) # load original $a1 value back into $a1
	lw $a2, 20($sp) # load original $a1 value back into $a2
	lw $a3, 16($sp) # load original $a1 value back into $a3
	
	addiu $a1, $a1, -1 # change $a1 from LenS to LenS - 1
	addiu $a3, $a3, -1 # change $a3 from LenT to LenT - 1
	jal levenshteinDistance
	move $t3, $v0
	lw $t0, 12($sp) # load original $t1 value back into $t1
	addu $t3, $t3, $t0
	sw $t3, 0($sp)
	
	lw $t1, 8($sp) # load original $t1 value into $t1
	lw $t2, 4($sp) # load original $t2 value into $t2
	lw $t3, 0($sp) # load original $t3 value into $t3
	move $a0, $t1 # move $t1 into $a0
	move $a1, $t2 # move $t1 into $a1
	move $a2, $t3 # move $t1 into $a2
	jal minimum
	
	addi $sp, $sp, 32 # restore the stack
	
	levenshteinReturn:
	lw $ra, 0($sp) # load the return address
	addi $sp, $sp, 4 # restore stack
	jr $ra # return to function caller
	
strlen:
	li $t0, 0 # this will hold the value for number of characters
	
	strLenLoop: 
	lbu $t1, 0($a0) # load the current byte at current address
	beqz $t1, terminateStrLenLoop # if the byte is 0, it is a null character, terminate the loop

	addi $t0, $t0, 1 # if this part was reached, that means character read wasn't null, increase length by 1
	addi $a0, $a0, 1 # go to address of next byte

	j strLenLoop # loop to see if next byte is null or not
	
	terminateStrLenLoop:
	move $v0, $t0 # move the length of the string to return value
	jr $ra # return to fuction caller

minimum:
	ble $a0, $a1, AlessB # if a < b, check if a < c
	ble $a1, $a2, BlessC # if this section is reached, then b < a, check if b < c
	move $v0, $a2 # if this section is reached, then a > b, and b > c; c is the smallest, put c in return value
	j gotMin # we got the smallest value

	BlessC:
	la $v0, ($a1) # put b in return value
	j gotMin # we got the smallest value

	AlessB:
	ble $a0, $a2, AlessC # if a < c, then a is smallest
	move $v0, $a2 # if this section is reached, a < b, but a > c; c is the smallest, put c in return value
	j gotMin # we got the smallest value

	AlessC:
	move $v0, $a0 # if this section is reached, a > b, and a > c; a is the smallest, put a in return value
	j gotMin # we got the smallest value

	gotMin:
	jr $ra # return to function caller

##########################################################################################################
#                                             Part 3                                                     #
##########################################################################################################
rd100Chars:
	addi $sp, $sp, -4 # make room to save $a0
	sw $a0, 0($sp) # save $a0 in case we need to call function again for invalid input
	addi $sp, $sp, -4 # make room to save $a1
	sw $a1, 0($sp) # save $a1 in case we need to call function again for invalid input
	la $t0, ($a0) # move $a0, to $t0; we need to use $a0 to hold the input prompt
	move $a0, $a1 # move input prompt into $a0
	li $v0, 4 # load print string syscall value
	syscall # print the input prompt
	
	move $a0, $t0 # move the m1 label back to $a0
	li $a1, 101 # read maximum of 100 characters
	li $v0, 8 # load read string syscall value
	syscall # read the string
	
	li $t0, 0 # loop counter
	li $t1, 100 # loop end value
	checkFor100: 
	beq $t0, $t1, read100Chars # compare program counter with end value
	lb $t2, 0($a0) # load a byte from the matrix string
	beqz $t2, not100Chars # if it is equal to zero, the string is not 100 chars
	addi $t0, $t0, 1 # increment counter by 1
	addi $a0, $a0, 1 # go to next byte of $a0
	j checkFor100 # loop again
	
	not100Chars:
	move $a0, $a2 # move input prompt into $a0
	li $v0, 4 # load print string syscall value
	syscall # print the input prompt
	lw $a1, 0($sp) # load original $a1, need to prompt input again
	addi $sp, $sp, 4 # restore stack
	lw $a0, 0($sp) # load original $a0, need to prompt input again
	addi $sp, $sp, 4 # restore stack
	j rd100Chars
	
	read100Chars:
	jr $ra

printIntMatrix:
	move $t3, $a0 # move $a0 into $t3, we will need to use $a3 to print
	li $t0, 0 # loop counter
	li $t1, 25 # loop end value
	li $t4, 5 # this loop counter checks to see when to print a new row
	
	printMatrix: 
	beq $t0, $t1, read25Elem # compare program counter with end value
	lw $t2, 0($t3) # load a word from the matrix string
	addi $t0, $t0, 1 # increment counter by 1
	addi $t3, $t3, 4 # go to next word of $a0
	# move the loaded word and print int value
	beqz $t4, printNewLine # if $t4 is 0, print new row
	
	printElement:
	move $a0, $t2 # move the word into $a0 to print
	li $v0, 1 # print the int value of the matrix element
	syscall # print int value of the matrix element
	
	#print a space
	la $a0, space # load the space label
	li $v0, 4 # call syscall value to print string
	syscall # print a space
	
	addi $t4, $t4, -1 # decrement the column counter by 1
	j printMatrix # loop again
	
	printNewLine:
	li $t4, 5 # reset the column counter
	la $a0, newLine # load the newLine label
	li $v0, 4 # load the syscall value for print string
	syscall # print the new line
	j printElement # print the element on a new line
	
	read25Elem:
	jr $ra # return to function caller

matrixMult:
	addi $sp, $sp, -4 # make space for return address
	sw $ra, 0($sp) # store the return address
	
	#################################################################################################
	#                                           base case                                           #
	#################################################################################################
	li $t0, 5 # load 5 into $t0 as loop comparative value
	
	lw $t1, I # load the word at label I
	lw $t2, J # load the word at label J
	lw $t3, K # load the word at label K
	bge $t1, $t0, matrixMultDone # if I >= 5, return
	
	#################################################################################################
	#                                           recursion                                           #
	#################################################################################################	
	bge $t2, $t0, incI # if J >= 5 don't do code below (equivalent to do code below if J < 5)
	bge $t3, $t0, incJ # if K >= 5 don't do code below (equivalent to do code below if K < 5)
	
	# get matrixA[i][k]
	la $t4, ($a0) # get the address of matrixA
	lw $t7, I # load I into $t7
	mul $t7, $t7, $t0 # multiply I by number of columns (5)
	lw $t8, K # load K into $t8
	addu $t7, $t7, $t8 # add I * 5 and K
	li $t8, 4 # load 4 into $t8
	mul $t7, $t7, $t8 # multiply (I * 5 + K) * 4
	addu $t4, $t4, $t7 # add to address of matrixA to get matrixA[i][k]	
	
	# get matrixB[k][j]
	la $t5, ($a1) # get the address of matrixB
	lw $t7, K # load K into $t7
	mul $t7, $t7, $t0 # multiply K by number of columns (5)
	lw $t8, J # load J into $t8
	addu $t7, $t7, $t8 # add K * 5 and J
	li $t8, 4 # load 4 into $t8
	mul $t7, $t7, $t8 # multiply (K * 5 + J) * 4
	addu $t5, $t5, $t7 # add to address of matrixB to get matrixB[k][j]
	
	# get matrixA[i][k] + matrixB[k][j]
	lw $t7, ($t4)
	lw $t8, ($t5)
	mul $t9, $t7, $t8
	
	# get result[i][j]
	la $t6, ($a2) # get the address of result
	lw $t7, I # load I into $t7
	mul $t7, $t7, $t0 # multiply I by number of columns (5)
	lw $t8, J # load J into $t8
	addu $t7, $t7, $t8 # add I * 5 and J
	li $t8, 4 # load 4 into $t8
	mul $t7, $t7, $t8 # multiply (I * 5 + J) * 4
	addu $t6, $t6, $t7 # add to address of result to get result[i][j]
	lw $t4, ($t6) # load the VALUE at this address matrixA[i][k] + matrixB[k][j]
	addu $t9, $t9, $t4 # add this value with the value of 
	sw $t9, ($t6) # store the word at $t6 into result[i][j]
	
	lw $t3, K # load value of K to increment
	addi $t3, $t3, 1 # increment value of L
	sw $t3, K # store incremented value of K
	jal matrixMult
	
	incJ:
	sw $zero, K # set K to zero
	lw $t2, J # load value of J to increment
	addi $t2, $t2, 1 # increment value of J
	sw $t2, J # store incremented value of J
	jal matrixMult
	
	incI:
	sw $zero, J # set J to zero
	lw $t1, I # load value of I to increment
	addi $t1, $t1, 1 # increment value of I
	sw $t1, I # store incremented value of I
	jal matrixMult	
	
	matrixMultDone:
	lw $ra, 0($sp) # load the return address
	addi $sp, $sp, 4 # restore the stack
	jr $ra # return to function caller

.data
printInvalidBase: .asciiz "Invalid base!"
.align 2
m1: .space 101
.align 2
m2: .space 101
.align 2
result: .space 101
.align 2
I: .word 0
J: .word 0
K: .word 0
newLine: .asciiz "\n"
space: .asciiz " "
