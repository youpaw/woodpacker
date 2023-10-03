BITS 64
global decrypt

%macro  genkey 1-2
	aeskeygenassist xmm2, xmm1, %1 ; round 2
	call key_expansion_128
	movdqu %2, xmm1
%endmacro

start:
	jmp decrypt

key_expansion_128: ; expand key from xmm2
	pshufd xmm2, xmm2, 0xff ; "shuffle packed double word"
	vpslldq xmm3, xmm1, 0x4 ; "shift double quadword left logical"
	pxor xmm1, xmm3
	vpslldq xmm3, xmm1, 0x4
	pxor xmm1, xmm3
	vpslldq xmm3, xmm1, 0x4
	pxor xmm1, xmm3
	pxor xmm1, xmm2
	ret

decrypt:
	mov r10, 0xdeadbeefdeadbeef ; HARD CODED encrypted segment length (encryption aligned)
	mov r9, 0xdeadbeefdeadbeef ; HARD CODED first part of the key
	mov r8, 0xdeadbeefdeadbeef ; HARD CODED second part of the key
	movq xmm1, r9
	movq xmm2, r8
	movlhps xmm1, xmm2 ; 0x00000000ffffffff to 0xffffffff00000000
	; get virt address of the encrypted segment
	lea rdi, [ rel start ]
	mov r11, r10
	neg r11
	add rdi, r11 ; relative offset to the beginning of encrypted segment
	; mprotect
	mov rax, 0xa ; syscall mprotect
	mov rsi, r10 ; size
	mov rdx, 0x07 ; protection
	syscall

	; aes thing
	mov rsi, rdi; encrypted segment offset
	mov rdx, r10; encrypted segment length
	movdqu xmm0, xmm1 ; move key in xmm0
	genkey 0x1, xmm4
	genkey 0x2, xmm5
	genkey 0x4, xmm6
	genkey 0x8, xmm7
	genkey 0x10, xmm8
	genkey 0x20, xmm9
	genkey 0x40, xmm10
	genkey 0x80, xmm11
	genkey 0x1b, xmm12
	genkey 0x36, xmm13
	aesimc xmm4, xmm4
	aesimc xmm5, xmm5
	aesimc xmm6, xmm6
	aesimc xmm7, xmm7
	aesimc xmm8, xmm8
	aesimc xmm9, xmm9
	aesimc xmm10, xmm10
	aesimc xmm11, xmm11
	aesimc xmm12, xmm12
	xor rdi, rdi

begin_loop:
	cmp rdx, rdi
	jle end

perform:
	movdqu xmm15, [rsi + rdi] ; move value in xmm15
	pxor xmm15, xmm13
	aesdec xmm15, xmm12
	aesdec xmm15, xmm11
	aesdec xmm15, xmm10
	aesdec xmm15, xmm9
	aesdec xmm15, xmm8
	aesdec xmm15, xmm7
	aesdec xmm15, xmm6
	aesdec xmm15, xmm5
	aesdec xmm15, xmm4
	aesdeclast xmm15, xmm0
	movdqu [rsi + rdi], xmm15
	add rdi, 0x10
	jmp begin_loop

end: ; reset registers
	xor rax, rax
	xor rdi, rdi
	xor rsi, rsi
	xor rdx, rdx
	xor rcx, rcx
	xor r8, r8
	xor r9, r9