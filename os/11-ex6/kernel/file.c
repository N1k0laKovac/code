#include "file.h"
#include "os.h"  // ����uart_putc����

// ���ڴ����ʵ��
#define HEAP_SIZE 0x10000
static char heap[HEAP_SIZE];
static char *heap_ptr = heap;

void *malloc(size_t size) {
    if (heap_ptr + size > heap + HEAP_SIZE) return NULL;
    void *ptr = heap_ptr;
    heap_ptr += size;
    return ptr;
}

void free(void *ptr) {
    // ��ʵ�ֲ������ڴ�
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n-- > 0)
        *p++ = (unsigned char)c;
    return s;
}

// �ļ�ϵͳʵ��
void fs_init(struct filesystem *fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        fs->files[i].used = 0;
        fs->files[i].data = NULL;
        fs->files[i].size = 0;
        memset(fs->files[i].name, 0, MAX_FILENAME_LEN);
    }
}

int fs_create(struct filesystem *fs, const char *name, const char *data, size_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs->files[i].used) {
            // ����ļ�����ͻ
            for (int j = 0; j < MAX_FILES; j++) {
                if (fs->files[j].used && strcmp(fs->files[j].name, name) == 0)
                    return -1;
            }
            
            // �����ڴ�
            if (!(fs->files[i].data = malloc(size))) return -1;
            
            // ��������
            strncpy(fs->files[i].name, name, MAX_FILENAME_LEN-1);
            memcpy(fs->files[i].data, data, size);
            fs->files[i].size = size;
            fs->files[i].used = 1;
            return 0;
        }
    }
    return -1;
}

int fs_cat(struct filesystem *fs, const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].used && strcmp(fs->files[i].name, name) == 0) {
            for (size_t j = 0; j < fs->files[i].size; j++)
                uart_putc(fs->files[i].data[j]);
            uart_putc('\n');
            return 0;
        }
    }
    uart_putc('e');
    return -1;
}

int fs_remove(struct filesystem *fs, const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].used && strcmp(fs->files[i].name, name) == 0) {
            free(fs->files[i].data);
            fs->files[i].data = NULL;
            fs->files[i].size = 0;
            fs->files[i].used = 0;
            memset(fs->files[i].name, 0, MAX_FILENAME_LEN);
            return 0;
        }
    }
    return -1;
}