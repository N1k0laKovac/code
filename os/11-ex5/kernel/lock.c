#include "os.h"
#include "riscv.h"

// 使用 amoswap.w 实现 Test & Set
int spin_lock(volatile int *lock) {
    int ret;
    do {
        // amoswap.w 原子交换操作
        asm volatile (
            "amoswap.w %0, %1, (%2)"
            : "=r"(ret)
            : "r"(1), "r"(lock)
            : "memory"
        );
    } while (ret != 0); // 如果返回1，说明锁已被占用
    return 0;
}

int spin_unlock(volatile int *lock) {
    *lock = 0; // 释放锁
    asm volatile ("fence rw, rw"); // 内存屏障
    return 0;
}