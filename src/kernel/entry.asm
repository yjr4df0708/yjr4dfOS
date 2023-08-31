format ELF
extrn main
public _start
_start:
	call main
	cli
	hlt
