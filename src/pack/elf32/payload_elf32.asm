BITS 32
global decrypt

%macro  genkey 1-2
	aeskeygenassist xmm2, xmm1, %1 ; round n
	call key_expansion_128
	movaps %2, xmm1
%endmacro

start:
    ; preserve
	push eax
	push edi
	push esi
	push edx
	push ecx
	call print_woody
    call decrypt
    pop ecx
    pop edx
    pop esi
    pop edi
    pop eax
	jmp 0xdeadbeef

print_woody:
    push ebp
    mov ebp, esp
    sub esp,0x10
    mov dword [ebp-4], `.\n\0\0`
    mov dword [ebp-8], `y...`
    mov dword [ebp-12], 'Wood'
    mov dword [ebp-16],'....'
    mov eax,0x4
    mov ebx, 0x1
    lea ecx,[ebp-16]
    mov edx, 0xe
    int 0x80
    leave
    ret

key_expansion_128: ; expand key from xmm2
	pshufd xmm2, xmm2, 0xff ; "shuffle packed double word"
	vpslldq xmm7, xmm1, 0x4 ; "shift double quadword left logical"
	pxor xmm1, xmm7
	vpslldq xmm7, xmm1, 0x4
	pxor xmm1, xmm7
	vpslldq xmm7, xmm1, 0x4
	pxor xmm1, xmm7
	pxor xmm1, xmm2
	ret

decrypt:
    push ebp
    mov ebp, esp
    sub esp,0x18
    mov dword [ebp-4], 0xdeadbeef ; HARD CODED encrypted segment size (encryption aligned)
    mov dword [ebp-12], 0xdeadbeef ; HARD CODED forth part of the key
    mov dword [ebp-16], 0xdeadbeef ; HARD CODED third part of the key
    mov dword [ebp-20], 0xdeadbeef ; HARD CODED second part of the key
    mov dword [ebp-24], 0xdeadbeef ; HARD CODED first part of the key

	; get virt address of the encryption key
	movups xmm1, [ebp-24]
	; get virt address of the encrypted segment
	; lea edi, [ start ] edi contains start address
	mov esi, [ebp-4]
	neg esi
	add edi, esi ; relative offset to the beginning of encrypted segment
	mov [ebp-8], edi ; backup encrypted segment offset
    ; align rdi for mprotect
    mov esi, edi
    and edi, -0x1000;
    ; get the alignement
    neg edi
    add esi, edi
    neg edi
    add esi, [ebp-4] ; text size + alignement
	; mprotect
	mov eax, 0x7d ; syscall mprotect
	mov ebx, edi ; address
	mov ecx, esi ; size
	mov edx, 0x07 ; protection
	int 0x80

	; aes thing
	mov edi, [ebp-8] ; encrypted segment offset
	mov esi, [ebp-4] ; encrypted segment size
	movaps xmm0, xmm1 ; move key in xmm0
	genkey 0x1, xmm3
	genkey 0x2, xmm4
	genkey 0x4, xmm5
	genkey 0x8, xmm6
	genkey 0x10, xmm7
	aesimc xmm3, xmm3
	aesimc xmm4, xmm4
	aesimc xmm5, xmm5
	aesimc xmm6, xmm6
	xor edx, edx

begin_loop:
	cmp esi, edx
	jle end

perform:
	movups xmm1, [edi+edx] ; move value in xmm15
	pxor xmm1, xmm7
	aesdec xmm1, xmm6
	aesdec xmm1, xmm5
	aesdec xmm1, xmm4
	aesdec xmm1, xmm3
	aesdeclast xmm1, xmm0
	movups [edi+edx], xmm1
	add edx, 0x10
	jmp begin_loop

end:
    leave
	ret
