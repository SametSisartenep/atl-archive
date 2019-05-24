DATA	s+0(SB)/8,$"hello wo"
DATA	s+8(SB)/4,$"rld\n"
GLOBL	s(SB),$13

TEXT greet(SB), $32
	MOVL $1, BP
	MOVQ $s+0(SB), DI
	MOVQ DI, 8(SP)
	MOVL $12, DI
	MOVL DI, 16(SP)
	CALL write(SB)
	RET
