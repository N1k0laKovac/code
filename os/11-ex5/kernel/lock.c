#include "os.h"
#include "riscv.h"

// 使用 amoswap.w 实现 Test & Set
int spin_lock(volatile int *lock) {
    int ret;
    do {
        // amoswap.w 原子交换操作
        asm volatile (
            "amoswap.w %0, %1, (%2)"// amoswap.w 原子地将 lock 指向的内存值与寄存器中的值 1 交换。
            : "=r"(ret)  // 输出：ret 保存内存中的旧值
            : "r"(1), "r"(lock)  // 输入：向内存写入 1，操作地址为 lock
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