	AREA test, CODE, READONLY
	
	export asm_dot_product
	
; Arguments: R0 = address of first elem of vector A
;			 R1 = address of first elem of vector B
;			 R2 = number of elements in vectors
;			 R3 = address of place to store result
; Description: Starting from the first elements in the array, compute the dot product
asm_dot_product
	push {r4}
	VPUSH.f32 {s0, s1, s2}
	VSUB.f32 s2, s2, s2 	; Set S2 to 0 by subtracting the register from itself
;	lsl r4, r2, #2 			; Quadruple # of items in array, to get mem positions
;	sub r4, r4, #4			; Decrement address to position 3996.
;	add r0, r0, r4			; Compute relative addresses
;	add r1, r1, r4			; Compute relative addresses
	mov r4, #0			
loop
;	subs r2, r2, #1 		; Decrement loop counter
	add r4, r4, #1
	VLDR.f32 s0, [r0] 
	VLDR.f32 s1, [r1]
	add r0, r0, #4
	add r1, r1, #4
	VMUL.f32 s1, s0, s1 	; Multiply 2 operands
	VADD.f32 s2, s2, s1 	; Accumulate dot product in S2
	CMP r4, r2
	blt loop
	
	VSTR.f32 s2, [r3]
	
	pop {r4}
	VPOP.f32 {s0, s1, s2}
	bx lr
	

	END
	
