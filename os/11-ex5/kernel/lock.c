#include "os.h"
#include "riscv.h"

// ʹ�� amoswap.w ʵ�� Test & Set
int spin_lock(volatile int *lock) {
    int ret;
    do {
        // amoswap.w ԭ�ӽ�������
        asm volatile (
            "amoswap.w %0, %1, (%2)"// amoswap.w ԭ�ӵؽ� lock ָ����ڴ�ֵ��Ĵ����е�ֵ 1 ������
            : "=r"(ret)  // �����ret �����ڴ��еľ�ֵ
            : "r"(1), "r"(lock)  // ���룺���ڴ�д�� 1��������ַΪ lock
            : "memory"
        );
    } while (ret != 0); // �������1��˵�����ѱ�ռ��
    return 0;
}

int spin_unlock(volatile int *lock) {
    *lock = 0; // �ͷ���
    asm volatile ("fence rw, rw"); // �ڴ�����
    return 0;
}