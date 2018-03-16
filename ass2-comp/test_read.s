.data
_t0:	.double 2.3
_t2:	.quad 898
_t1:	.string "\n"
_print_string:	.string "%s"
_print_double:	.string "%lg"
_print_long:	.string "%ld"
test:	.string "123.34"
	.lcomm _library_buf, 32
.text
.globl main

stoi:	# rdi = address buffer, rsi = read bytes
		testq %rsi, %rsi
		je .stoi_error
		xorq %rax, %rax # number
		xorq %r8, %r8 # i
		xorq %r9, %r9
		movq $10, %r10
		movb (%rdi, %r8), %r9b
		cmpq $48, %r9
		jb .stoi_error
		cmpq $57, %r9
		jg .stoi_error
.stoi_loop:
		subq $48, %r9
		mulq %r10
		addq %r9, %rax
		inc %r8
		movb (%rdi, %r8), %r9b
		cmpq $48, %r9
		jb .stoi_ret
		cmpq $57, %r9
		jg .stoi_ret
		jmp .stoi_loop
.stoi_ret:
		ret
.stoi_error:
        # exit(0)" << std::endl;
        movq $60, %rax # syscal call for exit" << std::endl;
        movq $1, %rdi # return code" << std::endl;
        decq %rdi
        syscall
        # enf of stoi


main: # main begin
blk0: # block blk0 begin
		

		movq $0, %rax # syscall number
		movq $0, %rdi # stdin file descriptor
		lea _library_buf, %rsi # address of the buffer
		movq $32, %rdx # size of the buffer
		syscall
		lea _library_buf, %rdi
		movq %rax, %rsi
		call stoi
		movq %rax, %rsi




		#lea test, %rax

		# Expand: := call printf(_t0) */
		subq $8, %rsp # Alignment
		lea _print_long, %rdi # Arg 1
		#movq _t2, %rsi # Arg 2
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
