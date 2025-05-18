#include "os.h"

#include "user_api.h"

#define DELAY 4000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");

	unsigned int hid = -1;

	/*
	 * if syscall is supported, this will trigger exception, 
	 * code = 2 (Illegal instruction)
	 */
	// hid = r_mhartid();
	// printf("hart id is %d\n", hid);


	int ret = -1;
	ret = gethid(&hid);
	// ret = gethid(NULL);
	if (!ret) {
		printf("system call returned!, hart id is %d\n", hid);
	} else {
		printf("gethid() failed, return: %d\n", ret);
	}

    // MyPrintf("Task 1: Hello via syscall!\n");
	// MyPrintf("Task 2: Hello via syscall!\n");
	// MyPrintf("Task 3: Hello via syscall!\n");

	while (1){
		uart_puts("Task 0: Running... \n");
		task_delay(DELAY);
	}
}

void user_task1(void)
{   
	MyPrintf("Task 1: Hello via syscall!\n");
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running... \n");
		
		task_delay(DELAY);
	}
}

void call_MyPrintf(){
	char* fmt = "NOT FUN AT ALL\n";
	MyPrintf(fmt);
	while (1) {
		task_delay(DELAY);
	}
}

void test(void){
	char* fmt = "NOT FUN AT ALL\n";
	MyPrintf(fmt);
	user_task0();
	MyPrintf(fmt);
}
void testAdd(void){
	char buf[2];  // 存放 "3" + '\0'
    buf[0] = '3';  // 直接赋值字符 '3'
    buf[1] = '\0'; // 字符串结束符
	
    MyPrintf(buf);  // 打印 "3"

	// MyPrintf((char*) 3);
	while (1) {
		task_delay(DELAY);
	}
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	// // MyPrintf("qq");
	// task_create(testAdd);
	// // task_create(call_MyPrintf);
	// // task_create(test);
	task_create(user_task0);
	task_create(user_task1);
}

