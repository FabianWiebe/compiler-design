.data
_t0:	.double 2.3
_t2:	.quad 898
_t1:	.string "\n"
_print_string:	.string "%s"
_print_double:	.string "%lg"
_print_long:	.string "%ld"
test:	.string "123.34"
	.lcomm buf, 32
.text
.globl main

fpconv:
		movq %rax, %r11
		movq %rdi, %rax
		lea buf, %rdi
		xorq %rcx, %rcx # index = 0
		movq $10, %rsi
		testq %r11, %r11 # check if long or double
		jz .fpconv_first_loop
		cvttsd2siq %xmm0, %rax # paramter in xmm0
.fpconv_first_loop:
		testq %rax, %rax # rax == 0
		jz .fpconv_done
		xorq %rdx, %rdx # zero out high part of div
		divq %rsi # / 10
		addb $48, %dl # + '0'
		movb %dl, (%rdi, %rcx) # push to buf
		incq %rcx # ++index
		jmp .fpconv_first_loop
.fpconv_done:
		movq %rcx, %rsi
		sarq %rsi
		decq %rcx # --index
		xorq %r8, %r8 # r8 = j
.fpconv_revers:
		testq %rsi, %rsi
		je .fpconv_end_revers
		movq %rcx, %r9 # copy index
		subq %r8, %r9 # r9 = index - j
		movb (%rdi, %r8), %al
		movb (%rdi, %r9), %dl
		movb %al, (%rdi, %r9)
		movb %dl, (%rdi, %r8)
		incq %r8 # ++j
		decq %rsi
		jmp .fpconv_revers
.fpconv_end_revers:
		incq %rcx # ++indx
		testq %r11, %r11 # check if long or double
		je .fpconv_ret # skip decimals, if long
		movb $46, (%rdi, %rcx) # dot
		incq %rcx
		movq $1, %rsi
		movq $10, %r8
		movq $6, %r9 # 5 decimal places
.fpconv_dec_loop:
		decq %r9
		jz .fpconv_ret
		movq %rsi, %rax
		mulq %r8
		movq %rax, %rsi
		cvtsi2sdq %rsi, %xmm1
		mulsd %xmm0, %xmm1 # paramter in xmm0
		cvttsd2siq %xmm1, %rax
		xorq %rdx, %rdx
		divq %r8
		addb $48, %dl
		movb %dl, (%rdi, %rcx) # push to buf
		incq %rcx # ++index
		jmp .fpconv_dec_loop
.fpconv_ret: 
		movb $0, (%rdi, %rcx) # null value
		movq %rdi, %rax # return value
		ret


main: # main begin
blk0: # block blk0 begin
		movq $1, %rax # Vec args
		movq _t2, %rdi # Vec args
		movsd _t0, %xmm0 # double Arg 1
		call fpconv

		#lea test, %rax

		# Expand: := call printf(_t0) */
		subq $8, %rsp # Alignment
		movq %rax, %rdi # Arg 1
		movq $0, %rax # Vec args
		call printf
		addq $8, %rsp # Alignment

		# Expand: := call printf(_t1) */
		subq $8, %rsp # Alignment
		lea _print_string, %rdi # Arg 1
		lea _t1, %rsi # Arg 2
		movq $0, %rax # Vec args
		call printf
		addq $8, %rsp # Alignment

		# block blk0 end

		ret # main end