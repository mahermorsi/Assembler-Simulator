.word 256 50								# load point a
.word 257 3634								# load point b
.word 258 3654								# load point c
add $gp, $zero, $imm, 256          		    # Global pointer MAX=256
lw $a0, $zero, $imm, 256					# get a from address 256
lw $a1, $zero, $imm, 257					# get b from address 257
lw $a2, $zero, $imm, 258            		# get c from address 258
add $t2,$zero,$imm,1						#t2=1
sll $sp, $t2, $imm, 11			    		# set $sp = 1 << 11 = 2048
add $sp, $sp, $imm, -10						# adjust stack for 3 items
sw $a0, $sp, $imm, 0						# save point a
sw $a1, $sp, $imm, 1						# save point b
sw $a2, $sp, $imm, 2						#save point c
jal $ra, $imm, $zero, helper-func
lw $a0, $sp, $imm, 0						# load a
lw $a1, $sp, $imm, 1						# load b
lw $a2, $sp, $imm, 2						# load c
lw $t2, $sp,$imm, 3                         # load area of ABC 
sw $t2, $sp,$imm, 7                         # store area of ABC  in sp7
jal $ra, $imm, $zero, DrawabLine
lw $a0, $sp, $imm, 0						# load a
lw $a1, $sp, $imm, 1						# load b	
lw $a2, $sp, $imm, 2						# load c
jal $ra, $imm, $zero, func
add $sp, $sp, $imm, 10						# release stack
halt $zero, $zero, $zero, 0					# halt

draw-down-columns:
	out $a0,$zero,$imm,20       			#point on address of monitor
	add $t1,$zero,$imm,255					# t1=255
	out $t1,$zero,$imm,21       			#draw white pixel
	add $t1,$zero,$imm,1        			#t1=1
	out $t1,$zero,$imm,22					#command 1 to write
	add $a0,$a0,$gp,0						# a0+=MAX  jump to next column
	bgt $ra,$a0,$a2,0  						#return if j>c
	beq $imm,$zero,$zero, draw-down-columns #loop back
helper-func:
	add $s0,$zero,$zero, 0					#s0=0
	add $s1,$zero,$zero, 0					#s1=0
	add $s2,$zero,$zero, 0					#s2=0
    add $t0,$a0,$zero, 0					#t0=a0=j
	add $t1,$a1,$zero, 0					#t1=a1
	add $t2,$a2,$zero, 0					#t2=a2
CalculateX-aria:							# a label to get the row number of point a
	blt $imm, $a0, $gp, secondx     		# a<MAX jump to calculate Xb in secondx label
	sub $a0,$a0,$gp,0						# a=a-MAX
	add $s0, $s0,$imm, 1					# Xa=Xa+1(number of row which a is currently in)
	beq $imm, $zero,$zero,CalculateX-aria 	#loop back
secondx:									#label used to get the row number of point b
	blt $imm, $a1, $gp, thirdx     			#branch to thirdx label if b<MAX
	sub $a1,$a1,$gp,0						# b=b-256 (jump to the upper row)
	add $s1, $s1,$imm, 1					# s1=Xb (s1 is the row number where b is currently in)
	beq $imm, $zero,$zero , secondx
thirdx:										# a label to get the row number of point c
	blt $imm, $a2, $gp, TriangleArea     	# branch to triangle area if c<MAX
	sub $a2,$a2,$gp,0						#c=c-256 (jump to the upper row)
	add $s2, $s2,$imm, 1
	beq $imm, $zero,$zero , thirdx  		#loop back
TriangleArea:
	sub $a0 , $s2 , $s1,0					#xc-xb=t0
	sub $s2 , $s0 , $s2 , 0					#s2=xa-xc
	sub $s1 , $s1 , $s0,0					#s1=xb-xa
	mul $a0 , $a0 , $t0 , 0					# a0= a * (Xc-Xb)
	mul $a1 , $t1 , $s2 , 0					# a1= b * (Xa-Xc)
	mul $a2 , $t2 , $s1 , 0					# a2= c * (Xb-Xa)
	add $a1 ,$a1, $a0 , 0
	add $v0 , $a1 , $a2 , 0    				#(a * (Xc - Xb) + b * (Xa - Xc) + c * (Xb - Xa))
	sw $v0, $sp, $imm, 3					# Store v0 (sum of 3 triangle's areas)	
	bgt $ra, $v0, $zero, 0					# if v0>0 return back to ra
	mul $v0, $v0, $imm, -1          		# area was negative, switch to positive
    sw $v0, $sp, $imm, 3					# save v0
	beq $ra, $zero,$zero, 0         		#return back to 
DrawabLine: 
	bgt $imm, $a0, $a1, DrawBCline     		#jump to draw b-c line if we are done (x>b)
	out $a0, $zero, $imm, 20				# update the address of pixel monitor
	add $s1, $zero, $imm, 255				# $s1 = 255
	out $s1, $zero, $imm, 21	        	# set pixel color to white
	add $s1, $zero, $imm, 1			    	# $s1 = 1
	out $s1, $zero, $imm, 22				# draw pixel
	add $a0 , $a0 , $gp,0					# jump to the next point in the column a-b
	beq $imm, $zero, $zero, DrawabLine		# loop
DrawBCline:
	bgt $ra, $a1, $a2, 0     				#jump back to return address
	out $a1, $zero, $imm, 20				# update the address of pixel monitor
	add $t0, $zero, $imm, 255				# $t1 = 255
	out $t0, $zero, $imm, 21	       	    # set pixel color to white
	add $t0, $zero, $imm, 1			   	    # $t1 = 1
	out $t0, $zero, $imm, 22				# draw pixel
	add $a1 , $a1 , $imm ,1
	beq $imm, $zero, $zero, DrawBCline		# and return
func:
    sw $ra, $sp, $imm, 6						# save position of PC 
	add $t0, $a0 , $imm, 1 						#x=a+1
    sw $t0, $sp, $imm, 5						# save x (x is the first loop integer)
loopRows:
    lw $t0, $sp, $imm, 5						# load $t0=x
	add $t0, $t0 , $gp ,0						#x=a+MAX+1
	sw $t0, $sp, $imm, 5						# store x after updating it 
	lw $ra, $sp, $imm, 6						# load position of PC 
    lw $a1, $sp, $imm, 1						#load point b
	bge $ra, $t0, $a1, 0						# x>=b jump to PC=$ra  (go to halt)
	add $s0, $t0, $zero, 0                   	# s0=j=x  start of second loop 
	sw $s0, $sp, $imm, 4						# store j value
    lw $a2, $sp, $imm, 2						# load point c
	lw $a1, $sp, $imm, 1						# load b
	sub $t1, $a2 ,$a1, 0						# t1=c-b
	add $t1, $t1, $t0, 0 						#t1=c-b+x 
    sw $t1, $sp, $imm, 9						# save $t1
loopcolumns:
    lw $t1, $sp, $imm, 9
	bgt $imm, $s0, $t1, loopRows				# jump to first loop if j>t1
    lw $a1, $sp, $imm, 1						#load a
    lw $a2, $sp, $imm, 2						#load b
	add $a0, $s0, $zero, 0 	           			#a=j
	jal $ra, $imm, $zero, helper-func			#calculates the area of JBC
	add $s2, $v0, $zero, 0  					# s2=calc(j,b,c)
	sw $s2, $sp, $imm, 8						#store the area of triangle jbc
	lw $a0, $sp, $imm, 0
	lw $a2, $sp, $imm, 2
	lw $s0, $sp, $imm, 4						# LOAD J
	add $a1, $s0, $zero, 0 
	jal $ra, $imm, $zero, helper-func			#calculate the area of JAC
	lw $s2, $sp, $imm, 8						#load s2
	add $s2, $s2, $v0, 0          				#s2=area of (jbc + jac)
	sw $s2, $sp, $imm, 8						#store the value
	lw $a1, $sp, $imm, 1						#load b
    lw $a0, $sp, $imm, 0						#load a
	lw $s0, $sp, $imm, 4						# load $s0 (s0=j)
	add $a2, $s0, $zero, 0  					# a2=j
	jal $ra, $imm, $zero, helper-func			#calculate the area of JBA
    lw $s2, $sp, $imm, 8						#load s2
	add $s2, $s2, $v0, 0						#s2=area(abj) + area(bjc)+ area(bja)
	lw $a2, $sp, $imm, 2						#a2=point a
	lw $s0, $sp, $imm, 4      					#load j
	lw $t0, $sp, $imm, 5      					#load x (first loop integer)
	lw $t2, $sp,$imm, 7                         # load area of ABC
	lw $a1, $sp, $imm, 1						#load b
	lw $a2, $sp, $imm, 2						#load c
	bne $imm , $s2 , $t2 , L1          		    # 
    lw $s0, $sp, $imm, 4						# load $s0
	add $a0,$s0,$zero, 0      					#a0=j
	jal $ra, $imm, $zero, draw-down-columns     # jump to draw a column 
	lw $a0, $sp, $imm, 0						#load a
	lw $s0, $sp, $imm, 4						# load $s0
	add $s0 ,$s0,$imm , 1						#j++
	sw $s0, $sp, $imm, 4						# save $s0
	beq $imm , $zero , $zero , loopcolumns      #
L1:
	add $t0,$s0,$zero, 0						#x=j
	sw $t0, $sp, $imm, 5						#load $t0=x
	beq $imm,$zero,$zero,loopRows				#jump back to first loop