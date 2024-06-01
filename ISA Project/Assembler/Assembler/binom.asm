.word 256 7
.word 257 3
add $t2, $zero, $imm, 1				# $t2 = 1
sll $sp, $t2, $imm, 11				# set $sp = 1 << 11 = 2048
lw $a0, $zero, $imm, 256			# get n from address 256
lw $a1, $zero, $imm, 257			# get k from address 257
jal $ra, $imm, $zero, Binome	    # calc $v0 = Binome(n,k)
sw $v0, $zero, $imm, 258			# store Binome(n,k) in 257
halt $zero, $zero, $zero, 0			# halt

Binome:
	add $sp, $sp, $imm, -4				# adjust stack for 3 items
	sw $s0, $sp, $imm, 3				# save $s0
	sw $ra, $sp, $imm, 2				# save return address
	sw $a1, $sp, $imm, 1				# save argument k
	sw $a0, $sp, $imm, 0				# save argument n
	ble $imm, $a1, $zero, L2            # if k<=0 return 1
	ble $imm, $a0, $a1, L2              # if n<=k return 1
	sub $a0, $a0, $imm, 1				# calculate n - 1
	jal $ra, $imm, $zero, Binome	    # calc $v0=Binome(n-1,k)
	add $s0, $v0, $zero, 0				# $s0 = Binome(n-1,k)
	sub $a1, $a1, $imm, 1				# calculate k - 1
	jal $ra, $imm, $zero, Binome		# calc Binom(n-1,k-1)
	add $v0, $v0, $s0, 0				# $v0 = Binome(n-1,k) + Binome(n-1,k-1)
    lw $s0, $sp, $imm, 3				# restore $s0
	lw $ra, $sp, $imm, 2				# restore return address
	lw $a1, $sp, $imm, 1				# restore argument
	lw $a0, $sp, $imm, 0				# restore argument
	add $sp, $sp, $imm, 4				# pop 4 items from stack
	beq $ra, $zero, $zero, 0            # jump to halt
L2:
	add $sp, $sp, $imm, 4				# pop 4 items from stack
	add $v0 , $zero,$imm ,1                # return 1
	beq $ra, $zero, $zero, 0			# and return