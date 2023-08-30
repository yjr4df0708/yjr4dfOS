load_kernel:
	mov byte [ERRC], "D"
	;#read disk dimensions
	mov ah, 8
	mov dl, [BOOT_DISK]
	int 0x13
	jnc .c1
	mov byte [ERRC+1], "a"
	jmp B_ERR
	.c1:
	inc dh
	mov [HEADS], dh
	and cl, 0x3f
	mov [SEC_TRK], cl

	;#init counter to 0
	xor bp, bp
	.loop:
	cmp bp, [K_SIZE]
	je .end
	mov bx, bp
	shl bx, 5;instead of mul 512
	mov es, bx;other *2^4 is here
	mov bx, kernel
	mov ax, bp
	inc ax;This is here to skip the bootloader.
	div byte [SEC_TRK]
	mov cl, ah;#sector=LBA%SEC_TRK+1
	inc cl
	xor ah, ah
	div byte [HEADS]
	mov dh, ah;#head=Temp%HEADS
	mov ch, al;#cylinder=Temp/HEADS
	mov al, 1;#read one at a time
	mov ah, 2
	mov dl, [BOOT_DISK]
	int 0x13
	jc .c2
	mov byte [ERRC+1], "b"
	jmp B_ERR
	.c2:
	test ah, ah
	jz .c3
	mov byte [ERRC+1], "c"
	jmp B_ERR
	.c3:
	inc bp
	jmp .loop

	.end:
	ret

SEC_TRK:
	db 0
HEADS:
	db 0
