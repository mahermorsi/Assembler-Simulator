add $gp, $zero, $imm, 128		# gp=128
add $t0, $zero , $imm , 0		#$t0=0
sll $sp, $t2, $imm, 8			# set $sp = 1 << 8 = 256
add $sp,$sp,$imm,-1
out $t0, $zero, $imm, 17        # Diskstatus free to receive 
add $s2, $zero, $zero, 0		#s2=0, counter to loop from 0 to 7
add $a0, $zero,$imm,7			# a0=7
add $t2 , $zero, $imm, 1000     # t2=1000
jal $ra, $imm, $zero, before-func		
halt $zero, $zero, $zero, 0		# halt

before-func:
	sw $ra,$sp,$zero,0				#store return address
func: 
	bgt $imm, $s2, $a0, L0			#jump to L1 if s2>7
	mul $t1, $s2,$gp,0				#t1=128*s2
	add $t2 , $zero, $imm, 1000     # t2=1000
	add $t2,$t2,$t1,0				# t2=1000+128*s2
	out $t2, $zero, $imm, 16        #diskbuffer points on PC 1000 in RAM
	out $s2, $zero, $imm, 15		# disksector points to sector 0
	add $t0, $zero, $imm, 1         # t2=1
	jal $ra,$imm,$zero, Loop		# check if disk is free not busy
	out $t0, $zero, $imm , 14		# read sector s2
	add $t0, $zero , $zero, 0		#$t0=0
	add $s2,$s2,$imm,1
	beq $imm, $zero,$zero,func		#loop back
	
Loop:
	in $s1, $zero, $imm, 17			#read diskstatus
	beq $ra, $zero, $s1, 0			#jump to perform out command if diskstatus is ready
	beq $imm,$zero,$zero,Loop		#wait until diskstatus is free to receive

L0:
	add $t2, $zero, $imm, 1000		#
	out $t2, $zero, $imm, 16        # diskbuffer = PC 1000
	add $s0 , $zero, $imm, 128      # s0=128
	add $s1, $zero, $imm, 8         # s1=8 
	add $t0 , $zero, $imm, -1       # t0=-1
L1:
	add $t1, $zero, $imm, 1     # j=1
	add $t0 , $t0, $imm, 1	 	# i++
	lw $a0, $t0, $imm, 1000		# get register in PC 1000+i
	bge $imm , $t0 , $s0 ,end 	#
L2:
	bge $imm , $t1 , $s1, L1    #
	mul $t2, $t1, $imm, 128     #$t2=j*128
	add $t2, $t2, $t0, 0        #$t2=j*128 + i
	lw $a1, $t2, $imm, 1000     #get register in pc 1000+i+j*128
	add $a0,$a0,$a1, 0 			#
	sw $a0, $t0, $imm, 1000     # store a0+a1 in pc 1000+i
	add $t1, $t1, $imm, 1		#j++
	beq $imm, $zero, $zero, L2  # loop
end:
	add $t0, $zero , $imm , 0		#$t0=0	
	add $t2 , $zero, $imm, 1000     # t2=1000
	out $t2, $zero, $imm, 16        #diskbuffer points on PC 1000 in RAM
	add $t0, $zero , $imm , 8		#$t0=8
	out $t0, $zero, $imm, 15		# disksector points to sector 8
	jal $ra,$imm,$zero, Loop		# check if disk is free not busy
	add $t2 , $zero, $imm, 2        # t2=2
	out $t2, $zero, $imm , 14		# write sector 0 to 
	lw $ra,$sp,$zero,0				#store return address
	add $sp,$sp,$imm,1				#release stack
	beq $ra, $zero, $zero, 0		# return to ra