.model flat,c
.code

glhTranslatef2_SSE_Aligned proc matrix:DWORD, xyzw:DWORD
	mov		eax, matrix
	mov		ecx, xyzw

	movaps	xmm0, [eax]						; copy first column
	movaps	xmm7, [ecx]

	movaps	xmm4, xmm7
	movaps	xmm1, [eax+16]					; copy second column
	shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits. xxxx
	movaps	xmm2, [eax+32]					; copy third column
	movaps	xmm5, xmm7
	movaps	xmm3, [eax+48]					; copy fourth column
	shufps	xmm5, xmm5, 01010101b			; second 32 bit copied to all 32 bits. yyyy
	shufps	xmm7, xmm7, 10101010b			; third 32 bit copied to all 32 bits. zzzz

	mulps	xmm0, xmm4						; This is matrix[0]*x, matrix[1]*x, matrix[2]*x, matrix[3]*x
	mulps	xmm1, xmm5						; This is matrix[4]*y, matrix[5]*y, matrix[6]*y, matrix[7]*y
	mulps	xmm2, xmm7						; This is matrix[8]*z, matrix[9]*z, matrix[10]*z, matrix[11]*z

	addps	xmm0, xmm1						; Add first and second
	addps	xmm0, xmm2						; Add previous and third
	addps	xmm0, xmm3						; Add previous and the last

	movaps	[eax+48], xmm0					; copy to matrix[12], matrix[13], matrix[14], matrix[15]
	ret
glhTranslatef2_SSE_Aligned endp

glhScalef2_SSE_Aligned proc matrix:DWORD, xyzw:DWORD
	mov		eax, matrix
	mov		ecx, xyzw

	movaps	xmm0, [eax]						; copy first column
	movaps	xmm7, [ecx]

	movaps	xmm4, xmm7
	shufps	xmm4, xmm4, 11000000b			; first 32 bit copied to position 0, 1, 2. last 32 bit copied to 3 (same position)
	movaps	xmm1, [eax+16]					; copy second column
	mulps	xmm0, xmm4						; matrix[0]*=x, matrix[1]*=x, matrix[2]*=x, matrix[3]*=w
	movaps	[eax], xmm0
	movaps	xmm4, xmm7
	shufps	xmm4, xmm4, 11010101b			; second 32 bit copied to position 0, 1, 2. last 32 bit copied to 3 (same position)
	movaps	xmm2, [eax+32]					; copy third column
	mulps	xmm1, xmm4						; matrix[4]*=y, matrix[5]*=y, matrix[6]*=y, matrix[7]*=w
	movaps	[eax+16], xmm1
	shufps	xmm7, xmm7, 11101010b			; third 32 bit copied to position 0, 1, 2. last 32 bit copied to 3 (same position)
	mulps	xmm2, xmm7						; matrix[8]*=z, matrix[9]*=z, matrix[10]*=z, matrix[11]*=w
	movaps	[eax+32], xmm2
	ret
glhScalef2_SSE_Aligned endp

end

