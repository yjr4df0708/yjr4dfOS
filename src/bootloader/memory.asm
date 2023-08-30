; I did look at https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
; I eventually just copied or rewrote parts of it
detect_memory:
	mov byte [ERRC], "M"
	;detect memory
	;setup variables
	memtable=0x7800
	mem_segments=memtable-1
	xor bp, bp
	mov byte [mem_segments], 0
	;initial
	mov eax, 0xe820
	xor ebx, ebx
	mov ecx, 24
	mov edx, 0x534d4150
	mov di, memtable
	mov dword [di+20], 1
	int 0x15
	jnc .c1
	mov byte [ERRC+1], "a"
	jmp B_ERR
	.c1:
	mov edx, 0x534d4150
	cmp eax, edx
	je .c2
	mov byte [ERRC+1], "b"
	jmp B_ERR
	.c2:
	
	.loop:
	;test last entry
	jcxz .skip
	cmp cl, 20
	jbe .nt
	test byte [di+20], 1
	je .skip

	.nt:
	mov ecx, [di+8]
	or ecx, [di+12]
	jz .skip
	inc bp
	add di, 24

	.skip:
	test ebx, ebx
	je .end

	;new entry
	mov eax, 0xe820
	mov ecx, 24
	mov dword [di+20], 1
	int 0x15
	jc .end
	mov edx, 0x534d4150
	jmp .loop

	.end:
	mov [mem_segments], bp
	ret
