#include <inc/types.h>
#include <inc/lib.h>
#include <inc/syscall.h>

void sys_cputs(const char *s, size_t len);
int sys_cgetc(void);
int sys_env_destroy(envid_t envid);
envid_t sys_getenvid(void);
