add $t2, $zero, $imm, 1				# $t2 = 1
sll $sp, $t2, $imm, 11				# set $sp = 1 << 11 = 2048
add $t2, $zero, $imm, 255			# $t2 = 255
jal $ra, $imm, $zero, func			# jump to func 
halt $zero, $zero, $zero, 0			# halt
func:
    add $t2, $t2, $imm, 1			# $t2 counter points on the index of the main looop 
	add $t1, $zero, $imm, 272		# $t1 is loop iteration length
	beq $imm , $t2 , $t1 , end      # if $t2 =272 end the loops 
	add $t0, $t2, $imm, 1			# second loop index=$t0 = main loop index
second-loop:
	beq $imm , $t0 , $t1 , func     # jump to main loop if second loop index  = 272 
	lw $s0, $t2, $imm, 0			# load word from t2 position 
	lw $s1, $t0, $imm, 0			# load word from t0 position
	ble $imm , $s0 , $s1 , helper   # helper function to add second loop index and jump to second loop again if a[main loop index]<a[second loop index] in another word is sorted 
	add $s2, $s0, $imm, 0			# $s2 = load word from t2 position 
	sw $s1, $t2, $imm, 0			# store the word in t0 position to t2 position
	sw $s0, $t0, $imm, 0			# store the word in t2 position to t0 position
	add $t0, $t0, $imm, 1           #add the second loop indec 
	beq $imm , $zero , $zero , second-loop   # jump to second loop
end:
	beq $ra, $zero, $zero, 0				 # and return
helper:
	add $t0, $t0, $imm, 1
	beq $imm , $zero , $zero , second-loop
		
.word 256 1
.word 257 20
.word 258 2
.word 259 4
.word 260 6
.word 261 5
.word 262 8
.word 263 7
.word 264 9
.word 265 11
.word 266 10
.word 267 13
.word 268 12
.word 269 15
.word 270 14
.word 271 16
               

          


       
