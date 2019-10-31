		lw 1 0 mcand
		lw 2 0 mplier
		lw 7 0 zero
		lw 3 0 one
		lw 4 0 zero
		lw 6 0 limit
loop 		nand 5 2 3
		nand 5 5 5
		beq 5 0 skip
 		add 7 1 7
skip		add 1 1 1 
		add 3 3 3 
		lw 5 0 one
		add 4 4 5
		beq 4 6 done
		beq 0 0 loop
done 		halt
mcand 		.fill 29562
mplier 		.fill 11834
zero 		.fill 0
one 		.fill 1
limit 		.fill 15
tr		.fill loop
as		.fill skip
vs		.fill done
