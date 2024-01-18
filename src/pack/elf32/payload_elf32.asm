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
	push ebx
	push ebp
	mov ebp, esp
	sub esp,0x14
	mov dword [ebp-4], 0xdeadbeef ; original text size
	mov dword [ebp-8], 0xdeadbeef ; compressed size
	mov dword [ebp-12], 0xdeadbeef ; encrypted size (with encryption padding)
	; [ebp-16] text segment off
	; [ebp-20] map off
	call print_woody
	; get virt address of the encrypted segment
	; lea edi, [ start ] edi contains start address
	mov esi, [ebp-12]
	neg esi
	add edi, esi ; relative offset to the beginning of encrypted segment
	mov [ebp-16], edi ; backup encrypted segment offset
    ; align rdi for mprotect
    mov esi, edi
    and edi, -0x1000;
    ; get the alignement
    neg edi
    add esi, edi
    neg edi
    add esi, [ebp-4] ; text size + alignement
	; mprotect
	mov eax, 0x7D ; syscall mprotect
	mov ebx, edi ; address
	mov ecx, esi ; size
	mov edx, 0x07 ; protection
	int 0x80
    call decrypt
    ; syscall mmap to allocate memory for decompressed data
    push ebp
    mov eax, 0xC0
    xor ebx, ebx
	mov ecx, [ebp-4]
    mov edx, 3
    mov esi, 34
    xor edi, edi
    xor ebp, ebp
	int 0x80
    pop ebp
    mov [ebp-20], eax ; backup mapped data
	; syscall mmap to allocate decompression buffer
	push ebp
    mov eax, 0xC0
    xor ebx, ebx
    mov ecx, 16810128
    mov edx, 3
    mov esi, 34
    xor edi, edi
    xor ebp, ebp
    int 0x80
    pop ebp
	mov edx, [ebp-20]
	mov esi, [ebp-8]
	mov edi, [ebp-16]
	push eax
	push edx
	push esi
	push edi
	call decompress
	; deinit decompression buffer
    mov ecx, 16810128
    mov ebx, [esp+12]
    mov eax, 0x5B
    int 0x80
	add esp, 16
	jmp load

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
	mov edi, [ebp-16] ; data offset
	mov esi, [ebp-12] ; data size
    push ebp
    mov ebp, esp
    sub esp,0x10
    mov dword [ebp-4], 0xdeadbeef ; HARD CODED forth part of the key
    mov dword [ebp-8], 0xdeadbeef ; HARD CODED third part of the key
    mov dword [ebp-12], 0xdeadbeef ; HARD CODED second part of the key
    mov dword [ebp-16], 0xdeadbeef ; HARD CODED first part of the key
	movups xmm1, [ebp-16]

	; aes thing
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
	jle decrypt_end

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

decrypt_end:
    leave
	ret

decompress:; Function begin
        push    ebp                                     ; 0000 _ 55
        mov     ebp, esp                                ; 0001 _ 89. E5
        push    edi                                     ; 0003 _ 57
        push    esi                                     ; 0004 _ 56
        push    ebx                                     ; 0005 _ 53
        sub     esp, 28                                 ; 0006 _ 83. EC, 1C
        mov     eax, dword [ebp+8H]                     ; 0009 _ 8B. 45, 08
        cmp     dword [eax], 407642370                  ; 000C _ 81. 38, 184C2102
        jz      l_002                                   ; 0012 _ 74, 0C
l_001:  mov     dword [ebp-18H], 0                      ; 0014 _ C7. 45, E8, 00000000
        jmp     l_019                                   ; 001B _ E9, 000001C1

l_002:  mov     dword [ebp-18H], 0                      ; 0020 _ C7. 45, E8, 00000000
        mov     dword [ebp-14H], 4                      ; 0027 _ C7. 45, EC, 00000004
l_003:  mov     eax, dword [ebp-14H]                    ; 002E _ 8B. 45, EC
        cmp     eax, dword [ebp+0CH]                    ; 0031 _ 3B. 45, 0C
        jnc     l_019                                   ; 0034 _ 0F 83, 000001A7
        mov     eax, dword [ebp+8H]                     ; 003A _ 8B. 45, 08
        mov     edi, dword [ebp-14H]                    ; 003D _ 8B. 7D, EC
        mov     edi, dword [eax+edi]                    ; 0040 _ 8B. 3C 38
        mov     eax, dword [ebp-14H]                    ; 0043 _ 8B. 45, EC
        lea     ebx, [eax+4H]                           ; 0046 _ 8D. 58, 04
        cmp     edi, 8421520                            ; 0049 _ 81. FF, 00808090
        ja      l_001                                   ; 004F _ 77, C3
        mov     edx, dword [ebp+8H]                     ; 0051 _ 8B. 55, 08
        mov     eax, dword [ebp+14H]                    ; 0054 _ 8B. 45, 14
        add     edx, ebx                                ; 0057 _ 01. DA
        lea     ecx, [eax+800000H]                      ; 0059 _ 8D. 88, 00800000
        lea     esi, [edx+edi]                          ; 005F _ 8D. 34 3A
l_004:  cmp     edx, esi                                ; 0062 _ 39. F2
        jz      l_005                                   ; 0064 _ 74, 09
        mov     al, byte [edx]                          ; 0066 _ 8A. 02
        inc     edx                                     ; 0068 _ 42
        inc     ecx                                     ; 0069 _ 41
        mov     byte [ecx-1H], al                       ; 006A _ 88. 41, FF
        jmp     l_004                                   ; 006D _ EB, F3

l_005:  lea     eax, [edi+ebx]                          ; 006F _ 8D. 04 1F
        xor     esi, esi                                ; 0072 _ 31. F6
        xor     edx, edx                                ; 0074 _ 31. D2
        mov     dword [ebp-14H], eax                    ; 0076 _ 89. 45, EC
l_006:  cmp     esi, edi                                ; 0079 _ 39. FE
        jge     l_016                                   ; 007B _ 0F 8D, 0000013D
        mov     eax, dword [ebp+14H]                    ; 0081 _ 8B. 45, 14
        lea     ecx, [esi+1H]                           ; 0084 _ 8D. 4E, 01
        movzx   ebx, byte [eax+esi+800000H]             ; 0087 _ 0F B6. 9C 30, 00800000
        mov     esi, ecx                                ; 008F _ 89. CE
        mov     byte [ebp-10H], bl                      ; 0091 _ 88. 5D, F0
        cmp     ebx, 15                                 ; 0094 _ 83. FB, 0F
        jle     l_011                                   ; 0097 _ 7E, 72
        sar     ebx, 4                                  ; 0099 _ C1. FB, 04
        mov     esi, ebx                                ; 009C _ 89. DE
        cmp     ebx, 15                                 ; 009E _ 83. FB, 0F
l_007:  jnz     l_008                                   ; 00A1 _ 75, 16
        mov     eax, dword [ebp+14H]                    ; 00A3 _ 8B. 45, 14
        inc     ecx                                     ; 00A6 _ 41
        movzx   ebx, byte [eax+ecx+7FFFFFH]             ; 00A7 _ 0F B6. 9C 08, 007FFFFF
        add     esi, ebx                                ; 00AF _ 01. DE
        cmp     ebx, 255                                ; 00B1 _ 81. FB, 000000FF
        jmp     l_007                                   ; 00B7 _ EB, E8

l_008:  lea     eax, [ecx+1H]                           ; 00B9 _ 8D. 41, 01
        mov     dword [ebp-20H], esi                    ; 00BC _ 89. 75, E0
        mov     dword [ebp-24H], eax                    ; 00BF _ 89. 45, DC
        lea     eax, [edx+1H]                           ; 00C2 _ 8D. 42, 01
        mov     dword [ebp-28H], eax                    ; 00C5 _ 89. 45, D8
        mov     eax, dword [ebp+14H]                    ; 00C8 _ 8B. 45, 14
        mov     bl, byte [eax+ecx+800000H]              ; 00CB _ 8A. 9C 08, 00800000
        mov     byte [eax+edx], bl                      ; 00D2 _ 88. 1C 10
        mov     eax, dword [ebp+14H]                    ; 00D5 _ 8B. 45, 14
        add     eax, ecx                                ; 00D8 _ 01. C8
        mov     dword [ebp-1CH], eax                    ; 00DA _ 89. 45, E4
l_009:  inc     dword [ebp-1CH]                         ; 00DD _ FF. 45, E4
        dec     dword [ebp-20H]                         ; 00E0 _ FF. 4D, E0
        jz      l_010                                   ; 00E3 _ 74, 15
        mov     eax, dword [ebp-1CH]                    ; 00E5 _ 8B. 45, E4
        sub     eax, ecx                                ; 00E8 _ 29. C8
        mov     ebx, eax                                ; 00EA _ 89. C3
        mov     eax, dword [ebp-1CH]                    ; 00EC _ 8B. 45, E4
        mov     al, byte [eax+800000H]                  ; 00EF _ 8A. 80, 00800000
        mov     byte [ebx+edx], al                      ; 00F5 _ 88. 04 13
        jmp     l_009                                   ; 00F8 _ EB, E3

l_010:  mov     edx, dword [ebp-28H]                    ; 00FA _ 8B. 55, D8
        dec     esi                                     ; 00FD _ 4E
        add     edx, esi                                ; 00FE _ 01. F2
        add     esi, dword [ebp-24H]                    ; 0100 _ 03. 75, DC
        cmp     edi, esi                                ; 0103 _ 39. F7
        jle     l_016                                   ; 0105 _ 0F 8E, 000000B3
l_011:  mov     eax, dword [ebp+14H]                    ; 010B _ 8B. 45, 14
        mov     ebx, edx                                ; 010E _ 89. D3
        movzx   ecx, byte [eax+esi+800000H]             ; 0110 _ 0F B6. 8C 30, 00800000
        lea     eax, [esi+2H]                           ; 0118 _ 8D. 46, 02
        mov     dword [ebp-1CH], eax                    ; 011B _ 89. 45, E4
        mov     eax, dword [ebp+14H]                    ; 011E _ 8B. 45, 14
        sub     ebx, ecx                                ; 0121 _ 29. CB
        movzx   ecx, byte [eax+esi+800001H]             ; 0123 _ 0F B6. 8C 30, 00800001
        mov     esi, dword [ebp-10H]                    ; 012B _ 8B. 75, F0
        shl     ecx, 8                                  ; 012E _ C1. E1, 08
        and     esi, 0FH                                ; 0131 _ 83. E6, 0F
        sub     ebx, ecx                                ; 0134 _ 29. CB
        mov     cl, byte [ebp-10H]                      ; 0136 _ 8A. 4D, F0
        and     ecx, 0FH                                ; 0139 _ 83. E1, 0F
        cmp     cl, 15                                  ; 013C _ 80. F9, 0F
        jnz     l_013                                   ; 013F _ 75, 20
        mov     esi, 15                                 ; 0141 _ BE, 0000000F
l_012:  inc     dword [ebp-1CH]                         ; 0146 _ FF. 45, E4
        mov     ecx, dword [ebp+14H]                    ; 0149 _ 8B. 4D, 14
        mov     eax, dword [ebp-1CH]                    ; 014C _ 8B. 45, E4
        movzx   ecx, byte [ecx+eax+7FFFFFH]             ; 014F _ 0F B6. 8C 01, 007FFFFF
        add     esi, ecx                                ; 0157 _ 01. CE
        cmp     ecx, 255                                ; 0159 _ 81. F9, 000000FF
        jz      l_012                                   ; 015F _ 74, E5
l_013:  mov     ecx, dword [ebp+14H]                    ; 0161 _ 8B. 4D, 14
        mov     dword [ebp-20H], esi                    ; 0164 _ 89. 75, E0
        add     ecx, ebx                                ; 0167 _ 01. D9
        mov     al, byte [ecx]                          ; 0169 _ 8A. 01
        mov     dword [ebp-10H], ecx                    ; 016B _ 89. 4D, F0
        mov     ecx, dword [ebp+14H]                    ; 016E _ 8B. 4D, 14
        mov     byte [ecx+edx], al                      ; 0171 _ 88. 04 11
        mov     al, byte [ecx+ebx+1H]                   ; 0174 _ 8A. 44 19, 01
        mov     byte [ecx+edx+1H], al                   ; 0178 _ 88. 44 11, 01
        mov     al, byte [ecx+ebx+2H]                   ; 017C _ 8A. 44 19, 02
        mov     byte [ecx+edx+2H], al                   ; 0180 _ 88. 44 11, 02
        lea     eax, [edx+4H]                           ; 0184 _ 8D. 42, 04
        mov     dword [ebp-24H], eax                    ; 0187 _ 89. 45, DC
        mov     al, byte [ecx+ebx+3H]                   ; 018A _ 8A. 44 19, 03
        mov     byte [ecx+edx+3H], al                   ; 018E _ 88. 44 11, 03
l_014:  dec     dword [ebp-20H]                         ; 0192 _ FF. 4D, E0
        inc     dword [ebp-10H]                         ; 0195 _ FF. 45, F0
        cmp     dword [ebp-20H], -1                     ; 0198 _ 83. 7D, E0, FF
        jz      l_015                                   ; 019C _ 74, 13
        mov     eax, dword [ebp-10H]                    ; 019E _ 8B. 45, F0
        sub     eax, ebx                                ; 01A1 _ 29. D8
        mov     ecx, eax                                ; 01A3 _ 89. C1
        mov     eax, dword [ebp-10H]                    ; 01A5 _ 8B. 45, F0
        mov     al, byte [eax+3H]                       ; 01A8 _ 8A. 40, 03
        mov     byte [ecx+edx+3H], al                   ; 01AB _ 88. 44 11, 03
        jmp     l_014                                   ; 01AF _ EB, E1

l_015:  mov     edx, dword [ebp-24H]                    ; 01B1 _ 8B. 55, DC
        add     edx, esi                                ; 01B4 _ 01. F2
        mov     esi, dword [ebp-1CH]                    ; 01B6 _ 8B. 75, E4
        jmp     l_006                                   ; 01B9 _ E9, FFFFFEBB

l_016:  cmp     esi, edi                                ; 01BE _ 39. FE
        jne     l_001                                   ; 01C0 _ 0F 85, FFFFFE4E
        mov     edi, dword [ebp-18H]                    ; 01C6 _ 8B. 7D, E8
        mov     esi, dword [ebp+14H]                    ; 01C9 _ 8B. 75, 14
        add     edi, dword [ebp+10H]                    ; 01CC _ 03. 7D, 10
        lea     ecx, [edi+edx]                          ; 01CF _ 8D. 0C 17
l_017:  cmp     edi, ecx                                ; 01D2 _ 39. CF
        jz      l_018                                   ; 01D4 _ 74, 03
        movsb                                           ; 01D6 _ A4
        jmp     l_017                                   ; 01D7 _ EB, F9

l_018:  add     dword [ebp-18H], edx                    ; 01D9 _ 01. 55, E8
        jmp     l_003                                   ; 01DC _ E9, FFFFFE4D
; decompress End of function

l_019:  ; Local function
        mov     eax, dword [ebp-18H]                    ; 01E1 _ 8B. 45, E8
        add     esp, 28                                 ; 01E4 _ 83. C4, 1C
        pop     ebx                                     ; 01E7 _ 5B
        pop     esi                                     ; 01E8 _ 5E
        pop     edi                                     ; 01E9 _ 5F
        pop     ebp                                     ; 01EA _ 5D
        ret                                             ; 01EB _ C3

load:
		; memcopy decompressed data back
		mov     edi, [ebp-16] ; text segment offset
		mov     ebx, [ebp-20] ; mapped data addr
        mov     ecx, [ebp-4] ; mapped data size
        mov     esi, 0
memcpy_loop:
		movzx   eax, byte [ebx+esi]
        mov     byte [edi+esi], al
        add     esi, 1
        cmp     esi, ecx
        jnz     memcpy_loop
        ; munmap decompressed data
		mov eax, 0x5B
		int 0x80
		add esp,0x14
		pop ebp
		pop ebx
		pop ecx
		pop edx
		pop esi
		pop edi
		pop eax
		jmp 0xdeadbeef
