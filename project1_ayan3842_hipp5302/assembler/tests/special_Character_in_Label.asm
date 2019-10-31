		lw	1, 0, v@lue1		# r1 <- v@lue1
		lw	2, 0, value2		# r2 <- value2
		beq	1, 2, done	# if (v@lue1 == value2) skip to done
done		halt
v@lue1		.fill 3
value2		.fill 3
