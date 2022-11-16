.text
.intel_syntax noprefix
.extern _start
.global start_external
.equ SYS_exit, 60

start_external:
call _start
mov rax, SYS_exit
syscall
