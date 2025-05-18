#include "syscall.h"
#ifndef __USER_API_H__
#define __USER_API_H__

/* user mode syscall APIs */
extern int gethid(unsigned int *hid);
extern int MyPrintf(const char *fmt, ...);
extern int task_create(void (*)(void));

// // ���������ţ����û�δʵ����Ĭ����ת��ϵͳ����
// int __attribute__((weak)) MyPrintf(const char *fmt, ...) {
//     // Ĭ����Ϊ��ֱ�ӵ���ϵͳ���ã���ȷ�� a7 �� a0 ����ȷ���ã�
//     register int a7 asm("a7") = SYS_MyPrintf;
//     register const char *a0 asm("a0") = fmt;
//     asm volatile("ecall" : : "r"(a7), "r"(a0));
//     return 0; // ʵ��Ӧ���ں˷���ֵ����
// }

#endif /* __USER_API_H__ */
