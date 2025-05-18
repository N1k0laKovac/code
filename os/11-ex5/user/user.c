#include "os.h"
#include "exit.h"
#include "user_api.h"
#include "syscall.h"

#define DELAY 4000

void task_delay(volatile int count)
{
	count *= 50000;
	while (count--);
}

// int shm_get() {
//     return syscall(SYS_SHM_GET, 0, 0, 0);
// }

// void P() {
//     syscall(SYS_P, 0, 0, 0);
// }

// void V() {
//     syscall(SYS_V, 0, 0, 0);
// }

// int get_tick() {
//     return syscall(SYS_GET_TICK, 0, 0, 0);
// }
// 用户任务示例
void user_task0(void) {
    char *shm = (char*)shm_get();
    while(1) {
        P();
        int current_tick = get_tick();
        strcpy_custom(shm, "Task0 write: ");
        char num_str[16];
        itoa(current_tick, num_str);
        strcat_custom(shm, num_str);
        V();
        task_delay(DELAY);
    }
}

void strcpy_custom(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void strcat_custom(char *dest, const char *src) {
    while (*dest) dest++;  // 找到 dest 的末尾
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

void itoa(int num, char *str) {
    int i = 0;
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);
    if (is_negative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    // 反转字符串
    for (int j = 0; j < i/2; j++) {
        char temp = str[j];
        str[j] = str[i-j-1];
        str[i-j-1] = temp;
    }
}

void user_task1(void) {
    char *shm = (char*)shm_get();
    while(1) {
        P();
        uart_puts("Received: ");
        uart_puts(shm);
        uart_puts("\n");
        V();
        task_delay(DELAY);
    }
}
// void call_MyPrintf(){
// 	char* fmt = "NOT FUN AT ALL\n";
// 	MyPrintf(fmt);
// 	while (1) {
// 		task_delay(DELAY);
// 	}
// }

// void test(void){
// 	char* fmt = "NOT FUN AT ALL\n";
// 	MyPrintf(fmt);
// 	user_task0();
// 	MyPrintf(fmt);
// }

// /* NOTICE: DON'T LOOP INFINITELY IN main() */
// void os_main(void)
// {
// 	uart_puts(">>> Kernel: os_main() reached\n");
//     while (1) {}
// 	 MyPrintf("qq");
// 	// task_create(call_MyPrintf);
// 	// task_create(test);
// 	// task_create(user_task0);
// 	// task_create(user_task1);
// }




int main() {
	MyPrintf("qq\n");
	// task_create(call_MyPrintf);
	// task_create(test);
	task_create(MyPrintf("qq\n"));
	task_create(user_task0);
	task_create(user_task1);
    uart_puts("Hello from main!\n");
	// MyPrintf("qq");
    exit(0);
}
