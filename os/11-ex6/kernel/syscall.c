#include "os.h"
#include "syscall.h"

extern struct filesystem fs;

char shared_mem[SHARED_MEM_SIZE];
volatile int shared_lock = 0;

int sys_gethid(unsigned int *ptr_hid)
{
	printf("--> sys_gethid, arg0 = %p\n", ptr_hid);
	if (ptr_hid == NULL) {
		return -1;
	} else {
		*ptr_hid = r_mhartid();
		return 0;
	}
}

static int _vsnprintf(char * out, size_t n, const char* s, va_list vl)
{
	int format = 0;
	int longarg = 0;
	size_t pos = 0;
	for (; *s; s++) {
		if (format) {
			switch(*s) {
			case 'l': {
				longarg = 1;
				break;
			}
			case 'p': {
				longarg = 1;
				if (out && pos < n) {
					out[pos] = '0';
				}
				pos++;
				if (out && pos < n) {
					out[pos] = 'x';
				}
				pos++;
			}
			case 'x': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				int hexdigits = 2*(longarg ? sizeof(long) : sizeof(int))-1;
				for(int i = hexdigits; i >= 0; i--) {
					int d = (num >> (4*i)) & 0xF;
					if (out && pos < n) {
						out[pos] = (d < 10 ? '0'+d : 'a'+d-10);
					}
					pos++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'd': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				long digits = 1;
				for (long nn = num; nn /= 10; digits++);
				for (int i = digits-1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (num % 10);
					}
					num /= 10;
				}
				pos += digits;
				longarg = 0;
				format = 0;
				break;
			}
			case 's': {
				const char* s2 = va_arg(vl, const char*);
				while (*s2) {
					if (out && pos < n) {
						out[pos] = *s2;
					}
					pos++;
					s2++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'c': {
				if (out && pos < n) {
					out[pos] = (char)va_arg(vl,int);
				}
				pos++;
				longarg = 0;
				format = 0;
				break;
			}
			default:
				break;
			}
		} else if (*s == '%') {
			format = 1;
		} else {
			if (out && pos < n) {
				out[pos] = *s;
			}
			pos++;
		}
    	}
	if (out && pos < n) {
		out[pos] = 0;
	} else if (out && n) {
		out[n-1] = 0;
	}
	return pos;
}

static char out_buf[1000]; // buffer for _vprintf()

static int _vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	uart_puts(out_buf);
	return res;
}

int sys_MyPrintf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = _vprintf(s, vl);
	va_end(vl);
	return res;
}


// 原子锁系统调用实现
int sys_shm_get() {
    return (int)shared_mem; // 返回共享内存地址
}

int sys_p() {
    return spin_lock(&shared_lock);
}

int sys_v() {
    return spin_unlock(&shared_lock);
}

int sys_get_tick() {
    return get_current_tick(); // 调用 timer.c 的接口
}

//文件系统实现
int sys_fs_create(const char *name, const char *data, size_t size) {
    // 手动分配内存（禁用标准库需自行实现内存管理）
    char *kernel_data = (char *)malloc(size);
    if (!kernel_data) return -1;

    // 复制数据到内核空间
    for (size_t i = 0; i < size; i++) {
        kernel_data[i] = data[i];
    }

    // 调用文件系统创建函数
    int ret = fs_create(&fs, name, kernel_data, size);

    // 释放内存（需实现自定义的 free）
    free(kernel_data);
    return ret;
}

int sys_fs_cat(const char *name) {
    return fs_cat(&fs, name);
}

int sys_fs_remove(const char *name) {
    return fs_remove(&fs, name);
}



void do_syscall(struct context *cxt)
{
	// printf(">>> do_syscall: a7 = %d\n", cxt->a7);
	uint32_t syscall_num = cxt->a7;

	switch (syscall_num) {
	case SYS_gethid:
		cxt->a0 = sys_gethid((unsigned int *)(cxt->a0));
		break;
	case SYS_MyPrintf:
	    // printf("Syscall MyPrintf: a0 = 0x%x\n", cxt->a0); 
		cxt->a0 = sys_MyPrintf((const char* )(cxt->a0));
		break;
	case SYS_task_create:
	    // printf("Syscall MyPrintf: a0 = 0x%x\n", cxt->a0); 
		cxt->a0 = sys_task_create((void (*)(void) )(cxt->a0));
		break;
	case SYS_SHM_GET:
        cxt->a0 = sys_shm_get();
        break;
    case SYS_P:
        cxt->a0 = sys_p();
        break;
    case SYS_V:
        cxt->a0 = sys_v();
        break;
	case SYS_GET_TICK:
        cxt->a0 = sys_get_tick();
        break;
	case SYS_FS_CREATE: {
            const char *name = (const char *)cxt->a0;
            const char *data = (const char *)cxt->a1;
            size_t size = (size_t)cxt->a2;
            cxt->a0 = sys_fs_create(name, data, size);
            break;
        }
        case SYS_FS_CAT: {
            const char *name = (const char *)cxt->a0;
            cxt->a0 = sys_fs_cat(name);
            break;
        }
        case SYS_FS_REMOVE: {
            const char *name = (const char *)cxt->a0;
            cxt->a0 = sys_fs_remove(name);
            break;
        }
	default:
		printf("Unknown syscall no: %d\n", syscall_num);
		cxt->a0 = -1;
	}

	return;
}