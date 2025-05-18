.section .text.entry  # 定义专属段名
.globl _user_start
_user_start:
    call main
    call exit