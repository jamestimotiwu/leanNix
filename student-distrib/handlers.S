# handlers.S - Interrupt handling routines for exceptions and interrupts

# Stack layout on entry before handling request
#  0(%esp) - %ebx
#  4(%esp) - %ecx
#  8(%esp) - %edx
#       C(%esp) - %esi
# 10(%esp) - %edi
# 14(%esp) - %ebp
# 18(%esp) - %eax
# 1C(%esp) - %ds
# 20(%esp) - %es
# 24(%esp) - orig_eax
# 28(%esp) - %eip
# 2C(%esp) - %cs
# 30(%esp) - %eflags
# 34(%esp) - %oldesp
# 38(%esp) - %oldss

save_reg:
    # Save registers to stack
    pushl %ds
    pushl %eax
    xorl %eax,%eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    decl %eax                       # Set eax to -1 for error
    pushl %ecx
    pushl %ebx