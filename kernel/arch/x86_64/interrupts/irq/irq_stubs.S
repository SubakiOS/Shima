.section .text
.code64

.extern irq_dispatch
.extern irq_entry_count


.macro IRQ num
.global irq\num

irq\num:
    .if \num == 32
        incq irq_entry_count(%rip)
    .endif
    pushq $0
    pushq $\num
    jmp irq_common_handler
.endm

IRQ 32
IRQ 33
IRQ 34
IRQ 35
IRQ 36
IRQ 37
IRQ 38
IRQ 39
IRQ 40
IRQ 41
IRQ 42
IRQ 43
IRQ 44
IRQ 45
IRQ 46
IRQ 47

.global irq_common_handler
.type irq_common_handler, @function

irq_common_handler:
    cld

    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, %r12

    movq 120(%rsp), %rdi
    subq $32, %rdi

    andq $-16, %rsp
    call irq_dispatch

    movq %r12, %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    addq $16, %rsp
    iretq

.section .note.GNU-stack,"",@progbits
