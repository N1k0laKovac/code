#include "os.h"
#include "exit.h"
#include "user_api.h"

#define DELAY 4000

// void user_task0(void)
// {
// 	uart_puts("Task 0: Created!\n");

// 	unsigned int hid = -1;

// 	/*
// 	 * if syscall is supported, this will trigger exception, 
// 	 * code = 2 (Illegal instruction)
// 	 */
// 	// hid = r_mhartid();
// 	// printf("hart id is %d\n", hid);


// 	int ret = -1;
// 	ret = gethid(&hid);
// 	// ret = gethid(NULL);
// 	if (!ret) {
// 		printf("system call returned!, hart id is %d\n", hid);
// 	} else {
// 		printf("gethid() failed, return: %d\n", ret);
// 	}

//     // MyPrintf("Task 1: Hello via syscall!\n");
// 	// MyPrintf("Task 2: Hello via syscall!\n");
// 	// MyPrintf("Task 3: Hello via syscall!\n");

// 	while (1){
// 		uart_puts("Task 0: Running... \n");
// 		task_delay(DELAY);
// 	}
// }

// void user_task1(void)
// {   
// 	MyPrintf("Task 1: Hello via syscall!\n");
// 	uart_puts("Task 1: Created!\n");
// 	while (1) {
// 		uart_puts("Task 1: Running... \n");
		
// 		task_delay(DELAY);
// 	}
// }

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
	// task_create(user_task0);
	// task_create(user_task1);
    uart_puts("Hello from main!\n");
	// MyPrintf("qq");
    exit(0);
}
