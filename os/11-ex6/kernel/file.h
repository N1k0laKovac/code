#define MAX_FILES 32
#define MAX_FILENAME_LEN 32
#define NULL ((void *)0)
typedef unsigned int size_t;

 // 添加自定义函数声明
// int strcmp(const char *s1, const char *s2);
char *strncpy(char *dest, const char *src, size_t n);
// void *memcpy(void *dest, const void *src, size_t n);
// void *memset(void *s, int c, size_t n);
void *malloc(size_t size);
void free(void *ptr);

struct file {
    char name[MAX_FILENAME_LEN];
    char *data;
    size_t size;
    int used;  // 使用标志位
};

struct filesystem {
    struct file files[MAX_FILES];  // 文件数组
};