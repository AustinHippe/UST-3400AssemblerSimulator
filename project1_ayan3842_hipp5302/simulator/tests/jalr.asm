	lw 2 0 four
	lw 1 0 six
	add 3 2 1
	lw 6 0 10
	jalr 7 6
	add 5 1 1
	beq 0 0 done
func	nand 4 1 2
	jalr 6 7
done	halt
faddr	.fill 7
four	.fill 2
six 	.fill 2