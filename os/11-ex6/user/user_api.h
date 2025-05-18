// user_api.h
#ifndef __USER_API_H__
#define __USER_API_H__

extern int gethid(unsigned int *hid);
extern int MyPrintf(const char *fmt, ...);
extern int task_create(void (*)(void));
extern int shm_get();
extern void P();
extern void V();
extern int get_tick();

#endif