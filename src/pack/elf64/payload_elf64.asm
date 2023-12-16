BITS 64
global decrypt

%macro  genkey 1-2
	aeskeygenassist xmm2, xmm1, %1 ; round n
	call key_expansion_128
	movdqu %2, xmm1
%endmacro

start:
    ; preserve
	push rax
	push rdi
	push rsi
	push rdx
	push rcx
	push r12
	push r13
	push r14
	push r15
	call print_woody
	; sub rsp, 0x40
	mov r13, 0xdeadbeefdeadbeef ; original text size
	mov r14, 0xdeadbeefdeadbeef ; compressed size
	mov r15, 0xdeadbeefdeadbeef ; encrypted size (with encryption padding)
	; get virt address of the encrypted segment
	lea rdi, [rel start]
	mov r11, r15
	neg r11
	add rdi, r11 ; relative offset to the beginning of encrypted segment
	mov r12, rdi ; backup offset
	; align rdi for mprotect
	mov r11, rdi
	and rdi, -0x10000000;
	; get the alignement
	neg rdi
	add r11, rdi
	neg rdi
	add r11, r13 ; alignement + original text size
	; mprotect
	mov rax, 0xa ; syscall mprotect
	mov rsi, r11 ; size
	mov rdx, 0x07 ; protection
	syscall
	mov rdi, r12
	mov rsi, r15
    call decrypt
	mov     r9, 0
	mov     r8, -1
	mov     r10, 34
	mov     rdx, 3
	mov     rsi, r13
	mov     rdi, 0
	mov		rax, 9 ; syscall mmap to allocate decompression buffer
	syscall
	push rax ; backup mapped data
	push r13 ; backup mapped data size
	push r12 ; backup segment offset
	mov rdi, r12
	mov rsi, r14
	mov rdx, rax
	mov rcx, r13
    call decompress
    jmp load

print_woody:
    push rbp
    mov rbp, rsp
    sub rsp,0x10
    mov dword [rbp-4], `.\n\0\0`
    mov dword [rbp-8], `y...`
    mov dword [rbp-12], 'Wood'
    mov dword [rbp-16],'....'
    mov rax,0x1
    mov rdi,0x1
    lea rsi,[rbp-16]
    mov rdx,0xe
    syscall
    leave
    ret

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
	; rdi - data offset
	; rsi - data size
	mov r9, 0xdeadbeefdeadbeef ; HARD CODED first part of the key
	mov r8, 0xdeadbeefdeadbeef ; HARD CODED second part of the key
	movq xmm1, r9
	movq xmm2, r8
	movlhps xmm1, xmm2 ; 0x00000000ffffffff to 0xffffffff00000000

	; aes thing
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
	xor rdx, rdx

begin_loop:
	cmp rsi, rdx
	jle decrypt_end
	movdqu xmm15, [rdi+rdx] ; move value in xmm15
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
	movdqu [rdi+rdx], xmm15
	add rdx, 0x10
	jmp begin_loop

decrypt_end:
	ret

decompress:
        push    r15                                     ; 0004 _ 41: 57
        push    r14                                     ; 0006 _ 41: 56
        push    r13                                     ; 0008 _ 41: 55
        push    r12                                     ; 000A _ 41: 54
        push    rbp                                     ; 000C _ 55
        push    rbx                                     ; 000D _ 53
        sub     rsp, 40                                 ; 000E _ 48: 83. EC, 28
        mov     rbx, rdi                                ; 0012 _ 48: 89. FB
        mov     r15, rsi                                ; 0015 _ 49: 89. F7
        mov     qword [rsp+10H], rsi                    ; 0018 _ 48: 89. 74 24, 10
        mov     qword [rsp+18H], rdx                    ; 001D _ 48: 89. 54 24, 18
        mov     r9, 0                                  ; 0022 _ 41: B9, 00000000
        mov     r8, -1                         ; 0028 _ 41: B8, FFFFFFFF
        mov     r10, 34                                 ; 002E _ B9, 00000022
        mov     rdx, 3                                  ; 0033 _ BA, 00000003
        mov     rsi, 16810128                           ; 0038 _ BE, 01008090
        mov     rdi, 0                                  ; 003D _ BF, 00000000
        mov		rax, 9 ; syscall mmap to allocate decompression buffer
        syscall
        mov     rdi, rax                                ; 0047 _ 48: 89. C7
        cmp     dword [rbx], 407642370                  ; 004A _ 81. 3B, 184C2102
        jne     l_021                                   ; 0050 _ 0F 85, 0000026E
        cmp     r15, 4                                  ; 0056 _ 49: 83. FF, 04
        jbe     l_023                                   ; 005A _ 0F 86, 0000028B
        mov     qword [rsp+8H], 0                       ; 0060 _ 48: C7. 44 24, 08, 00000000
        mov     r12d, 4                                 ; 0069 _ 41: BC, 00000004
        lea     r13, [rax+2H]                           ; 006F _ 4C: 8D. 68, 02
        lea     rbp, [rax+5H]                           ; 0073 _ 48: 8D. 68, 05
        jmp     l_018                                   ; 0077 _ E9, 000001F9

l_001:  lea     edx, [rax+1H]                           ; 007C _ 8D. 50, 01
        lea     r10d, [r9+1H]                           ; 007F _ 45: 8D. 51, 01
        movsxd  r14, r9d                                ; 0083 _ 4D: 63. F1
        lea     r8d, [rax+800000H]                      ; 0086 _ 44: 8D. 80, 00800000
        movsxd  r8, r8d                                 ; 008D _ 4D: 63. C0
        movzx   r8d, byte [rdi+r8]                      ; 0090 _ 46: 0F B6. 04 07
        mov     byte [rdi+r14], r8b                     ; 0095 _ 46: 88. 04 37
        cmp     esi, 1                                  ; 0099 _ 83. FE, 01
        je      l_010                                   ; 009C _ 0F 84, 0000015F
        movsxd  rdx, edx                                ; 00A2 _ 48: 63. D2
        add     rdx, rdi                                ; 00A5 _ 48: 01. FA
        movsxd  r15, eax                                ; 00A8 _ 4C: 63. F8
        lea     r10, [r13+r15]                          ; 00AB _ 4F: 8D. 54 3D, 00
        lea     r8d, [rsi-2H]                           ; 00B0 _ 44: 8D. 46, FE
        add     r10, r8                                 ; 00B4 _ 4D: 01. C2
        mov     dword [rsp+4H], eax                     ; 00B7 _ 89. 44 24, 04
l_002:  mov     r8, rdx                                 ; 00BB _ 49: 89. D0
        sub     r8, r15                                 ; 00BE _ 4D: 29. F8
        movzx   eax, byte [rdx+800000H]                 ; 00C1 _ 0F B6. 82, 00800000
        mov     byte [r8+r14], al                       ; 00C8 _ 43: 88. 04 30
        add     rdx, 1                                  ; 00CC _ 48: 83. C2, 01
        cmp     rdx, r10                                ; 00D0 _ 4C: 39. D2
        jnz     l_002                                   ; 00D3 _ 75, E6
        mov     eax, dword [rsp+4H]                     ; 00D5 _ 8B. 44 24, 04
        lea     eax, [rax+rsi]                          ; 00D9 _ 8D. 04 30
        lea     r9d, [rsi+r9]                           ; 00DC _ 46: 8D. 0C 0E
l_003:  cmp     r11d, eax                               ; 00E0 _ 41: 39. C3
        jle     l_014                                   ; 00E3 _ 0F 8E, 0000014D
l_004:  lea     edx, [rax+800000H]                      ; 00E9 _ 8D. 90, 00800000
        movsxd  rdx, edx                                ; 00EF _ 48: 63. D2
        movzx   esi, byte [rdi+rdx]                     ; 00F2 _ 0F B6. 34 17
        mov     edx, r9d                                ; 00F6 _ 44: 89. CA
        sub     edx, esi                                ; 00F9 _ 29. F2
        lea     r10d, [rax+2H]                          ; 00FB _ 44: 8D. 50, 02
        lea     esi, [rax+800001H]                      ; 00FF _ 8D. B0, 00800001
        movsxd  rsi, esi                                ; 0105 _ 48: 63. F6
        movzx   esi, byte [rdi+rsi]                     ; 0108 _ 0F B6. 34 37
        shl     esi, 8                                  ; 010C _ C1. E6, 08
        sub     edx, esi                                ; 010F _ 29. F2
        and     ecx, 0FH                                ; 0111 _ 83. E1, 0F
        cmp     ecx, 15                                 ; 0114 _ 83. F9, 0F
        je      l_011                                   ; 0117 _ 0F 84, 000000EE
l_005:  movsxd  rsi, edx                                ; 011D _ 48: 63. F2
        movsxd  r14, r9d                                ; 0120 _ 4D: 63. F1
        movzx   eax, byte [rdi+rsi]                     ; 0123 _ 0F B6. 04 37
        mov     byte [rdi+r14], al                      ; 0127 _ 42: 88. 04 37
        lea     eax, [rdx+1H]                           ; 012B _ 8D. 42, 01
        cdqe                                            ; 012E _ 48: 98
        movzx   r8d, byte [rdi+rax]                     ; 0130 _ 44: 0F B6. 04 07
        lea     eax, [r9+1H]                            ; 0135 _ 41: 8D. 41, 01
        cdqe                                            ; 0139 _ 48: 98
        mov     byte [rdi+rax], r8b                     ; 013B _ 44: 88. 04 07
        lea     eax, [rdx+2H]                           ; 013F _ 8D. 42, 02
        cdqe                                            ; 0142 _ 48: 98
        movzx   r8d, byte [rdi+rax]                     ; 0144 _ 44: 0F B6. 04 07
        lea     eax, [r9+2H]                            ; 0149 _ 41: 8D. 41, 02
        cdqe                                            ; 014D _ 48: 98
        mov     byte [rdi+rax], r8b                     ; 014F _ 44: 88. 04 07
        lea     eax, [rdx+4H]                           ; 0153 _ 8D. 42, 04
        lea     r8d, [r9+4H]                            ; 0156 _ 45: 8D. 41, 04
        add     edx, 3                                  ; 015A _ 83. C2, 03
        movsxd  rdx, edx                                ; 015D _ 48: 63. D2
        movzx   r15d, byte [rdi+rdx]                    ; 0160 _ 44: 0F B6. 3C 17
        lea     edx, [r9+3H]                            ; 0165 _ 41: 8D. 51, 03
        movsxd  rdx, edx                                ; 0169 _ 48: 63. D2
        mov     byte [rdi+rdx], r15b                    ; 016C _ 44: 88. 3C 17
        lea     r15d, [rcx-1H]                          ; 0170 _ 44: 8D. 79, FF
        test    ecx, ecx                                ; 0174 _ 85. C9
        je      l_013                                   ; 0176 _ 0F 84, 000000B2
        cdqe                                            ; 017C _ 48: 98
        add     rax, rdi                                ; 017E _ 48: 01. F8
        lea     r8, [rbp+rsi]                           ; 0181 _ 4C: 8D. 44 35, 00
        lea     edx, [rcx-1H]                           ; 0186 _ 8D. 51, FF
        add     r8, rdx                                 ; 0189 _ 49: 01. D0
l_006:  movzx   ecx, byte [rax]                         ; 018C _ 0F B6. 08
        mov     rdx, rax                                ; 018F _ 48: 89. C2
        sub     rdx, rsi                                ; 0192 _ 48: 29. F2
        mov     byte [rdx+r14], cl                      ; 0195 _ 42: 88. 0C 32
        add     rax, 1                                  ; 0199 _ 48: 83. C0, 01
        cmp     rax, r8                                 ; 019D _ 4C: 39. C0
        jnz     l_006                                   ; 01A0 _ 75, EA
        lea     r9d, [r15+r9+5H]                        ; 01A2 _ 47: 8D. 4C 0F, 05
l_007:  cmp     r10d, r11d                              ; 01A7 _ 45: 39. DA
        jge     l_015                                   ; 01AA _ 0F 8D, 00000089
l_008:  lea     eax, [r10+1H]                           ; 01B0 _ 41: 8D. 42, 01
        lea     edx, [r10+800000H]                      ; 01B4 _ 41: 8D. 92, 00800000
        movsxd  rdx, edx                                ; 01BB _ 48: 63. D2
        movzx   ecx, byte [rdi+rdx]                     ; 01BE _ 0F B6. 0C 17
        movzx   esi, cl                                 ; 01C2 _ 0F B6. F1
        cmp     esi, 15                                 ; 01C5 _ 83. FE, 0F
        jle     l_004                                   ; 01C8 _ 0F 8E, FFFFFF1B
        sar     esi, 4                                  ; 01CE _ C1. FE, 04
        cmp     esi, 15                                 ; 01D1 _ 83. FE, 0F
        jne     l_001                                   ; 01D4 _ 0F 85, FFFFFEA2
        lea     edx, [r10+2H]                           ; 01DA _ 41: 8D. 52, 02
        movsxd  rdx, edx                                ; 01DE _ 48: 63. D2
l_009:  mov     eax, edx                                ; 01E1 _ 89. D0
        movzx   r8d, byte [rdi+rdx+7FFFFFH]             ; 01E3 _ 44: 0F B6. 84 17, 007FFFFF
        add     esi, r8d                                ; 01EC _ 44: 01. C6
        add     rdx, 1                                  ; 01EF _ 48: 83. C2, 01
        cmp     r8d, 255                                ; 01F3 _ 41: 81. F8, 000000FF
        jz      l_009                                   ; 01FA _ 74, E5
        jmp     l_001                                   ; 01FC _ E9, FFFFFE7B

l_010:  mov     eax, edx                                ; 0201 _ 89. D0
        mov     r9d, r10d                               ; 0203 _ 45: 89. D1
        jmp     l_003                                   ; 0206 _ E9, FFFFFED5

l_011:  add     eax, 3                                  ; 020B _ 83. C0, 03
        cdqe                                            ; 020E _ 48: 98
l_012:  mov     r10d, eax                               ; 0210 _ 41: 89. C2
        movzx   esi, byte [rdi+rax+7FFFFFH]             ; 0213 _ 0F B6. B4 07, 007FFFFF
        add     ecx, esi                                ; 021B _ 01. F1
        add     rax, 1                                  ; 021D _ 48: 83. C0, 01
        cmp     esi, 255                                ; 0221 _ 81. FE, 000000FF
        jz      l_012                                   ; 0227 _ 74, E7
        jmp     l_005                                   ; 0229 _ E9, FFFFFEEF

l_013:  mov     r9d, r8d                                ; 022E _ 45: 89. C1
        jmp     l_007                                   ; 0231 _ E9, FFFFFF71

l_014:  mov     r10d, eax                               ; 0236 _ 41: 89. C2
l_015:  cmp     r10d, r11d                              ; 0239 _ 45: 39. DA
        jne     l_021                                   ; 023C _ 0F 85, 00000082
        movsxd  r9, r9d                                 ; 0242 _ 4D: 63. C9
        mov     rcx, qword [rsp+18H]                    ; 0245 _ 48: 8B. 4C 24, 18
        add     rcx, qword [rsp+8H]                     ; 024A _ 48: 03. 4C 24, 08
        test    r9, r9                                  ; 024F _ 4D: 85. C9
        jz      l_020                                   ; 0252 _ 74, 6B
        mov     eax, 0                                  ; 0254 _ B8, 00000000
l_016:  movzx   edx, byte [rdi+rax]                     ; 0259 _ 0F B6. 14 07
        mov     byte [rcx+rax], dl                      ; 025D _ 88. 14 01
        add     rax, 1                                  ; 0260 _ 48: 83. C0, 01
        cmp     r9, rax                                 ; 0264 _ 49: 39. C1
        jnz     l_016                                   ; 0267 _ 75, F0
l_017:  add     qword [rsp+8H], rax                     ; 0269 _ 48: 01. 44 24, 08
        cmp     r12, qword [rsp+10H]                    ; 026E _ 4C: 3B. 64 24, 10
        jnc     l_022                                   ; 0273 _ 73, 62
l_018:  mov     r11d, dword [rbx+r12]                   ; 0275 _ 46: 8B. 1C 23
        add     r12, 4                                  ; 0279 _ 49: 83. C4, 04
        cmp     r11d, 8421520                           ; 027D _ 41: 81. FB, 00808090
        ja      l_021                                   ; 0284 _ 77, 3E
        movsxd  rax, r11d                               ; 0286 _ 49: 63. C3
        lea     rsi, [rbx+r12]                          ; 0289 _ 4A: 8D. 34 23
        test    rax, rax                                ; 028D _ 48: 85. C0
        jz      l_017                                   ; 0290 _ 74, D7
        mov     edx, 0                                  ; 0292 _ BA, 00000000
l_019:  movzx   ecx, byte [rsi+rdx]                     ; 0297 _ 0F B6. 0C 16
        mov     byte [rdi+rdx+800000H], cl              ; 029B _ 88. 8C 17, 00800000
        add     rdx, 1                                  ; 02A2 _ 48: 83. C2, 01
        cmp     rax, rdx                                ; 02A6 _ 48: 39. D0
        jnz     l_019                                   ; 02A9 _ 75, EC
        add     r12, rdx                                ; 02AB _ 49: 01. D4
        mov     r10d, 0                                 ; 02AE _ 41: BA, 00000000
        mov     r9d, 0                                  ; 02B4 _ 41: B9, 00000000
        jmp     l_008                                   ; 02BA _ E9, FFFFFEF1

l_020:  mov     rax, r9                                 ; 02BF _ 4C: 89. C8
        jmp     l_017                                   ; 02C2 _ EB, A5

l_021:  mov     rsi, 16810128                           ; 02C4 _ BE, 01008090
		mov		rax, 11
        syscall ; deinit decompression buffer
        mov     qword [rsp+8H], 0                       ; 02CE _ 48: C7. 44 24, 08, 00000000
l_022:  mov     rax, qword [rsp+8H]                     ; 02D7 _ 48: 8B. 44 24, 08
        add     rsp, 40                                 ; 02DC _ 48: 83. C4, 28
        pop     rbx                                     ; 02E0 _ 5B
        pop     rbp                                     ; 02E1 _ 5D
        pop     r12                                     ; 02E2 _ 41: 5C
        pop     r13                                     ; 02E4 _ 41: 5D
        pop     r14                                     ; 02E6 _ 41: 5E
        pop     r15                                     ; 02E8 _ 41: 5F
        ret                                             ; 02EA _ C3
; decompress End of function

l_023:  ; Local function
        mov     qword [rsp+8H], 0                       ; 02EB _ 48: C7. 44 24, 08, 00000000
        jmp     l_022                                   ; 02F4 _ EB, E1

load:
		pop rdx ; text segment offset
		pop rsi ; mapped data size
		pop rdi ; mapped data addr
		; memcopy decompressed data back
        mov     rcx, 0
memcpy_loop:
		movzx   r8d, byte [rdi+rcx]
        mov     byte [rdx+rcx], r8b
        add     rcx, 1
        cmp     rcx, rsi
        jnz     memcpy_loop
        ; munmap decompressed data
		mov rax, 11
		syscall
		pop r15
		pop r14
		pop r13
		pop r12
		pop rcx
		pop rdx
		pop rsi
		pop rdi
		pop rax
		jmp 0xdeadbeef
