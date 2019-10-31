	lw 1 0 five	# load reg1 with 5 (symbolic address)
	lw 2 1 3	# load reg2 with -1 (numeric address)
	jalr 3 2        # load PC+1 to reg3 and assign value of PC to reg2
five    .fill 5
