#include "../inc/lib.h"

void write(int fd, const void *buf, size_t nbytes)
{
    sys_cputs(buf, nbytes);
}

int read(int fd, void *buf, size_t nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++) {
        int c = sys_cgetc();
        if (c < 0) {
            return c;
        }
        ((char *)buf)[i] = c;
    }
    return i;
}

envid_t getpid(void)
{
    return sys_getenvid();
}

void exit(void)
{
    return sys_env_destroy(0);
}



