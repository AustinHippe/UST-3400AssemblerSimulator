	lw 1 0 five	# load reg1 with 5 (symbolic address)
	lw 2 1 3	# load reg2 with -1 (numeric address)
start   add 1 2 1	# decrement reg1
	beq 0 1 2	# goto end of program when reg1==0
	beq 0 0 start	# go back to the beginning of the loop
	noop
done    halt		# end of program
five    .fill six
neg1    .fill -1
stAdd   .fill start	# will contain the address of start (2)
