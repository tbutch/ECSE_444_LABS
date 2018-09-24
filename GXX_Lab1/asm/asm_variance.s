	AREA test, CODE, READONLY
		
	
	export asm_variance
		
; Arguments: R0 = address of first elem of vector
;			 R1 = number of elements in vector
;			 R2 = address of place to store result
; Description: Starting from the first element in the array, compute the mean then the variance	
asm_variance

	push {r4,r5,r6}			; Save state
	VPUSH.f32 {s0,s1,s2,s3,s4}
	
	VSUB.f32 s0, s0, s0		; Reset S0 to 0. Mean is stored here
	VMOV.f32 s2, r1 		; Load S2 with the float value of the # of items in the array
	VCVT.f32.s32 s2,s2		; Cast Int to Float
	mov r4,#0				; Initialize counter
	mov r5, r0				; save initial array address in r5
	
mean
	add r4, r4, #1			; Go through array forwards
	VLDR.f32 s3, [r5]		; load number from memory
	add r5, r5, #4			; Increment address pointer
	VADD.f32 s0, s0, s3		; add numbers together
	CMP r4, r1				; compare to see which position we are at
	blt mean				; If number less than amount in array, branch
	
	VDIV.f32 s0,s0,s2		; divide result by # of items in array
	
	mov r4, #0				; Reset counter
	mov r5, r0				; Reset address
	VSUB.f32 s1, s1, s1 	; Reset S1 to 0. Variance is stored here
var
	add r4, r4, #1			; Increment counter
	VLDR.f32 s3,[r5]		; Load item from vector
	add r5, r5,#4			; Increment address
	VSUB.f32 s3, s3, s0		; Subtract mean from sample point
	VMLA.f32 s3, s3, s3		; Square sample point
	VADD.f32 s1, s3, s1		; Accumulate sum
	CMP r4, r1				; Check if counter still less than number of items in array
	blt var					; Branch if not gone through all items
	
	mov r6, #1				; Move 1 into a register
	VMOV.f32 s4, r6			; Move int 1 to float register s4
	VCVT.f32.s32 s4, s4		; Convert int 1 to float 1
	VSUB.f32 s2, s2, s4		; Remove 1 from # of items in array
	VDIV.f32 s1, s1, s2		; divide accumulated sum by N-1 items
	VSTR.f32 s1, [r2]		; store result
	
	pop{r4,r5,r6}			; Restore state
	VPOP.f32 {s0,s1,s2,s3,s4}
	bx lr
	
	END
		
