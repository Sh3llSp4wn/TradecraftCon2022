# define text segment and lang
.text
.intel_syntax noprefix

# global symbols we call in the c part
.global _write
.global _read
.global _open
.global _exit

# linux syscalls
.equ SYS_read, 0
.equ SYS_write, 1
.equ SYS_open, 2
.equ SYS_exit, 60
.equ SYS_mmap, 90
.equ SYS_munmap, 91

# just load rax with the number and syscall
.macro direct_syscall syscall_number, symbol
\symbol:
mov rax, \syscall_number
syscall
ret
# end the macro
.endm 

# generate the code
direct_syscall SYS_write, _write
direct_syscall SYS_read, _read
direct_syscall SYS_open, _open
direct_syscall SYS_munmap, _deallocate
direct_syscall SYS_mmap, _allocate

# exit is special because no return
# I will save this byte even if it kills me
_exit:
mov rax, SYS_exit
syscall
