# generated by usys.pl - do not edit
#include "syscall.h"
.global MyPrintf
MyPrintf:
 li a7, SYS_MyPrintf
 ecall
 ret
.global gethid
gethid:
 li a7, SYS_gethid
 ecall
 ret
.global task_create
task_create:
 li a7, SYS_task_create
 ecall
 ret
