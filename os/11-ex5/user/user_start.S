.section .text.entry  # ����ר������
.globl _user_start
_user_start:
    call main
    call exit