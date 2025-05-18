#include "syscall.h"
#ifndef __USER_API_H__
#define __USER_API_H__

/* user mode syscall APIs */
extern int gethid(unsigned int *hid);
extern int MyPrintf(const char *fmt, ...);
extern int task_create(void (*)(void));

// // 定义弱符号，若用户未实现则默认跳转至系统调用
// int __attribute__((weak)) MyPrintf(const char *fmt, ...) {
//     // 默认行为：直接调用系统调用（需确保 a7 和 a0 已正确设置）
//     register int a7 asm("a7") = SYS_MyPrintf;
//     register const char *a0 asm("a0") = fmt;
//     asm volatile("ecall" : : "r"(a7), "r"(a0));
//     return 0; // 实际应由内核返回值覆盖
// }

#endif /* __USER_API_H__ */
