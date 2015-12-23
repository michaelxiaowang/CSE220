.text
.globl main
main:

########################################################################################################################################################################################################
# main1 ################################################################################################################################################################################################
########################################################################################################################################################################################################

toRadians:
	l.s $f4, pi # load pi into $f4
	li $t0, 0x43340000 # stores IEEE 754 value of 180 to $t0
	mtc1 $t0, $f5 # moves value in $t0 to $f5
	mul.s $f12, $f12, $f4 # multiply the degrees by pi
	div.s $f0, $f12, $f5 # divide the result by 180, and put the result in $f0
	
	jr $ra # go to address of line after function was called

arcLength:
	#store its return address on stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	
	jal toRadians # call toRadians, $f12 already has saved degree value
	mul.s $f0, $f0, $f13 # multiply radians by radius, radians stored in $f0 and radius is stored in $f13, put final result in $f0
	
	#restore stack
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	
	jr $ra # go to address of line after function was called
	
########################################################################################################################################################################################################
# main2 ################################################################################################################################################################################################
########################################################################################################################################################################################################

atof:
	li $t0, 0 # loads 0 into $t0
	mtc1 $t0, $f0 # moves 0 into $f0
	li $t1, 0 # this register will hold the sign: 0 = postive; 1 = negative

	# this checks first char for a sign value
	lb $t0, ($a0)  # load the byte at the address input+offset
	beqz $t0, endOfParse # if the byte is null, stop parsing

	li $t8, 0x2B # holds the value of '+'
	beq $t0, $t8, gotSign # if '+', go to gotSign
	li $t9, 0x2D # holds the value of '-'
	beq $t0, $t9, setNeg # if '-', go to setNeg
	j checkSpecial # else jump to gotSign

	setNeg:
		li $t1, 1 # if '-', set $t1 to 1

	gotSign:
		addi $a0, $a0, 1
		lb $t0, ($a0)  # load the byte at the address input+offset
		beqz $t0, endOfParse # if the byte is null, stop parsing
		
	checkSpecial:
		li $t8, 0x49 # holds the value of 'I'
		li $t9, 0x4E # holds the value of 'N'
		
		beq $t0, $t8, checkInf # if 'I', go to checkInf
		beq $t0, $t9, checkNaN # if 'N', go to checkNaN

	startOfValue:
		li $t8, 0x41200000 # loads IEEE 754 value of 10
		mtc1 $t8, $f4 # this register will hold the multiplier
		valueLoop:
			li $t9, 0x2E # holds '.'
			lb $t0, ($a0)  # load the byte at the address input+offset
			addi $a0, $a0, 1 # get address of next byte
			beqz $t0, endOfParse # if the byte is null, stop parsing
			beq $t0, $t9, startOfFraction # if the char is '.', go to startOfFraction
			li $t8, 0x30 # load '0'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x31 # load '1'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x32 # load '2'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x33 # load '3'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x34 # load '4'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x35 # load '5'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x36 # load '6'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x37 # load '7'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x38 # load '8'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			li $t8, 0x39 # load '9'
			beq $t0, $t8, parseDigit # if valid digit, parse it
			j endOfParse # if not valid char, stop parsing
			
			parseDigit:
				li $t9, 0x30
				sub $t0, $t0, $t9 # get the decimal value of the character digit
				mtc1 $t0, $f6 # store the decimal value in $f6
				cvt.s.w $f6, $f6 # convert $f5 to single precision
				mul.s $f0, $f0, $f4 # multiply current value by 10
				add.s $f0, $f0, $f6 # add new digit to it
				j valueLoop
			
	startOfFraction:
		li $t8, 0x3dcccccd # loads IEEE 754 value of .1
		mtc1 $t8, $f4 # this register will hold the multiplier
		mtc1 $t8, $f5 # this register will hold the multiplier
		fractionLoop:
			lb $t0, ($a0)  # load the byte at the address input+offset
			beqz $t0, endOfParse # if the byte is null, stop parsing
			addi $a0, $a0, 1 # get address of next byte
			li $t8, 0x30 # load '0'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x31 # load '1'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x32 # load '2'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x33 # load '3'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x34 # load '4'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x35 # load '5'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x36 # load '6'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x37 # load '7'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x38 # load '8'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			li $t8, 0x39 # load '9'
			beq $t0, $t8, parseFloat # if valid digit, parse it
			j endOfParse # if not valid char, stop parsing
			
			parseFloat:
				li $t9, 0x30
				sub $t0, $t0, $t9 # get the decimal value of the character digit #f4 is multiplier #f5 is 0.1 #f6 is digit #f0 is total
				mtc1 $t0, $f6 # store the decimal value in $f6
				cvt.s.w $f6, $f6 # convert $f6 to single precision
				mul.s $f6, $f4, $f6 # multiply current value multiplier
				add.s $f0, $f0, $f6 # add new digit to it
				mul.s $f4, $f4, $f5 # get new multiplier
				j fractionLoop

	checkInf:
		addi $a0, $a0, 1 # get address of next byte
		lb $t0, ($a0)  # load the byte at the address input+offset
		li $t9, 0x6E # holds the value of 'n'
		bne $t0, $t9, endOfParse # if the byte is not 'n', stop parsing
		addi $a0, $a0, 1 # get address of next byte
		lb $t0, ($a0)  # load the byte add the address input+offset
		li $t9, 0x66 # holds the value of 'f'
		bne $t0, $t9, endOfParse # if the byte is not 'f', stop parsing
		li $t0, 0x7F800000 # sets value to Inf
		InfToFloat:
			mtc1 $t0, $f0
		j endOfParse # jump to endOfParse

	checkNaN:
		addi $a0, $a0, 1 # get address of next byte
		lb $t0, ($a0)  # load the byte at the address input+offset
		li $t9, 0x61 # holds the value of 'a'
		bne $t0, $t9, endOfParse # if the byte is null, stop parsing
		addi $a0, $a0, 1 # get address of next byte
		lb $t0, ($a0)  # load the byte add the address input+offset
		li $t9, 0x4E # holds the value of 'N'
		bne $t0, $t9, endOfParse # if the byte is not 'N', stop parsing
		li $t0, 0x7FFFFFFF # sets value to NaN
		NaNToFloat:
			mtc1 $t0, $f0

	endOfParse:
		li $t9, 0 # load 0, which is positive
		beqz $t1, done # if positive, then value is correct
		
		#If sign was negative multiply by -1
		li $t8, -1
		mtc1 $t8, $f4
		cvt.s.w $f4, $f4
		mul.s $f0, $f0, $f4		

	done:
		jr $ra

print_parts:
	mfc1 $t0, $f12 # move the argument value to $t0
	
	#print the Sign label
	la $a0, Sign # load sign label to $a0
	li $v0, 4 # print string
	syscall # print sign label
	srl $t1, $t0, 31 # shift right 31 bits
	
	# print single precision sign bit
	la  $a0, ($t1) # load value holding the first bit of argument
	li $v0, 1 # print int
	syscall # print sign bit
	
	#print space
	la  $a0, space
	li $v0, 4
	syscall 
		
	# prints the decimal sign
	beqz $t1, printPos # if positive, jump to positive print
	la $a0, negative # load negative label, holds "-"
	li $v0, 4 # print string
	syscall # print '-'
	printPos:
		la  $a0, positive  # load positive label, holds "+"
		li $v0, 4 # print string
		syscall # print '+'

	# print the exponent
	la $a0, Exponent # loads the Exponent label
	li $v0, 4 # print string
	syscall # print Exponent label
	sll $t1, $t0, 1 # shift left 1 bit; remove sign bit
	srl $t1, $t1, 24 # shift right 24 bits; remove fraction
	sll $t2, $t1, 24 # shift left 24 bits, to print each bit in a loop
	li $t3, 8 # loop counter, 8 iterations
	li $t4, 0x80000000 # load a comparative value
	
	loopExp:		
		beqz $t3, printExp # if loop counter is 0, print decimal exponent value
		bgeu  $t2, $t4, printe1 # if bit is greater than 1, print 1
		
		# else print 0
		li $a0, 0
		li $v0, 1
		syscall
		j shiftExp # jump to shiftExp
		
		#print 1
		printe1:
			li $a0, 1
			li $v0, 1
			syscall
			
		shiftExp:
			sll $t2, $t2, 1 # shift the bits left 1
		addi $t3, $t3, -1 # decrement counter
		j loopExp # repeat loop

	printExp:			
	# print space
	la  $a0, space
	li $v0, 4
	syscall
	
	# print decimal value of exponent
	la $a0, ($t1)
	li $v0, 1
	syscall
	
	# print the fraction
	la $a0, Fraction # load the Fraction label
	li $v0, 4 # print string call
	syscall # print Fraction label
	sll $t1, $t0, 1 # shift left 1 bit; get rid of sign bit
	srl $t1, $t1, 24 # shift right 24 bits; get the exponent value - test for Inf or NaN
	sll $t2, $t0, 9 # get fraction part with msb at left; to print binary value 1 bit at a time
	srl $t5, $t2, 9 # get fraction part with lsb at right; to print decimal value
	li $t3, 23 # loop counter; 23 iterations
	li $t4, 0x80000000 # comparative value
	
	loopFrac:		
		beqz $t3, printFrac # if loop counter is 0, go to printFrac
		bgeu  $t2, $t4, printf1 # if bit is greater than comparator (current bit is 1), print 1
		
		# else print 0
		li $a0, 0
		li $v0, 1
		syscall
		j shiftFrac # jump to shiftFrac
		
		# print 1
		printf1:
			li $a0, 1
			li $v0, 1
			syscall
			
		shiftFrac:
			sll $t2, $t2, 1 # shift bits over 1; evaluate next bit
		addi $t3, $t3, -1 # decrement loop counter
		j loopFrac # loop again

	printFrac:			
	# print space
	la  $a0, space
	li $v0, 4
	syscall
	
	# print decimal value of fraction
	li $t9, 0xFF # loads a comparative value
	beq $t1, $t9, noDecValue # compares the exponent to 0xFF, which is value for Inf or NaN, if equal jump to noDecValue
	
	# else print decimal value of fraction
	la $a0, ($t5)
	li $v0, 1
	syscall
	
	noDecValue:
	
	jr $ra # return to line after function call	
	
print_binary_product:
	# print BinaryProduct label
	la $a0, BinaryProduct # load the BinaryProduct label
	li $v0, 4 # print string call
	syscall # print Fraction label
	
	# print the mantissa
	mfc1 $t0, $f12 # move binary representation to $t0
	sll $t1, $t0, 1 # get rid of sign bit
	srl $t1, $t1, 24 # extract exponent
	sll $t2, $t0, 9 # get fraction part with msb at left; to print binary value 1 bit at a time
	li $t3, 23 # loop counter; 23 iterations; unless trailing 0s found
	li $t4, 0x80000000 # comparative value
	li $t6, 0x7F # comparative exponent value
	
	loopMant:		
		beqz $t3, printMant # if loop counter is 0, go to printFrac
		beqz $t2, printMant # if trailing 0s, go to printFrac
		bgeu  $t2, $t4, printm1 # if bit is greater than comparator (current bit is 1), print 1
		
		# else print 0
		li $a0, 0
		li $v0, 1
		syscall
		j shiftMant # jump to shiftFrac
		
		# print 1
		printm1:
			li $a0, 1
			li $v0, 1
			syscall
			
		shiftMant:
			sll $t2, $t2, 1 # shift bits over 1; evaluate next bit
		addi $t3, $t3, -1 # decrement loop counter
		j loopMant # loop again

	printMant:			
		# print BinaryProduct2 label
		la $a0, BinaryProduct2 # load the BinaryProduct2 label
		li $v0, 4 # print string call
		syscall # print Fraction label
		
		# extract actual exponent value
		li $t7, -1 
		mul $t7, $t6, $t7
		add $t7, $t1, $t7
	
		# print exponent of binary product
		bltz $t7, negativeExp # compares the exponent to 0x80, which is excess exponent value, if equal jump to noDecValue
		la  $a0, positive  # load positive label, holds "+"
		li $v0, 4 # print string
		syscall # print '+'
	
	negativeExp:
		la $a0, ($t7)
		li $v0, 1
		syscall
	
	#print 2 new line
	la $a0, newLine
	li $v0, 4
	syscall
	syscall
	
	jr $ra # return to line after function call

arcLengthS:
	#store its return address on stack
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	
	jal atof # call atof on the string
	mov.s $f13, $f0
	jal arcLength
	
	#restore stack
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	
	jr $ra # return to line after function call


########################################################################################################################################################################################################
# main3 ################################################################################################################################################################################################
########################################################################################################################################################################################################

product:
	#save $sp address
	la $t9, ($sp)
	
	# initialize $f0 as 1
	li $t0, 1
	mtc1 $t0, $f0 
	cvt.s.w $f0, $f0
	
	# initialize $f1 as 0
	li $t1, 0
	mtc1 $t1, $f2
	cvt.s.w $f2, $f2
		
	bgt $a0, $t0, multLoop # if number of values > 1 go to multLoop
	beq $a0, $t0, eq1 # if only 1 value, go to eq1
	
	# else value is 0
	mov.s $f0, $f2
	j finalProd
		
	# return the value
	eq1:
		l.s $f1, 0($sp) # get value from stack
		addi $sp, $sp, 4 # go to next value
		mul.s $f0, $f0, $f1 # multiply current product with new value to get new product
		j finalProd
	
	multLoop:
		beqz $a0, finalProd # if number of items left = 0, done
		l.s $f1, 0($sp) # get value from stack
		addi $sp, $sp, 4 # go to next value
		mul.s $f0, $f0, $f1 # multiply current product with new value to get new product
		
		addi $a0, $a0, -1 # decrement number of items
		j multLoop # loop again
	
	finalProd:
	la $sp, ($t9) #restore the stack pointer
	jr $ra

.data
Sign: .asciiz "\nSign: "
Exponent: .asciiz "\nExponent: "
Fraction: .asciiz "\nFraction: "
BinaryProduct: .asciiz "\nBinary Product: 1."
BinaryProduct2: .asciiz " x 2^"
newLine: .asciiz "\n"
space: .asciiz " "
positive: .asciiz "+"
negative: .asciiz "-"
entered: .asciiz "Entered"
