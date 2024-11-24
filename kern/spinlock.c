#include <kern/spinlock.h>
#include <inc/assert.h>
#include <inc/x86.h>

#define NLOCK 1000

static int nlock;
static struct spinlock *locks[NLOCK];

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->nts = 0;
  lk->n = 0;
  if(nlock >= NLOCK)
    panic("initlock");
  locks[nlock] = lk;
  nlock++;
}

int
holding(struct spinlock *lk)
{
  int r;
  r = (lk->locked);
  return r;
}

void
acquire(struct spinlock *lk)
{
    cli();
    
    if(holding(lk))
        panic("acquire");

    while (1) {
        if (__sync_bool_compare_and_swap(&lk->locked, 0, 1)) {
            break;
        }

        asm volatile("pause" ::: "memory");
        
        for (int i = 0; i < 1000; i++) {
            asm volatile("pause" ::: "memory");
        }
    }
    
    // memory barrier
    __sync_synchronize();
}

void release(struct spinlock *lk)
{
    if(!holding(lk))
        panic("release");
    
    // memory barrier
    __sync_synchronize();
    
    // release the lock
    lk->locked = 0;
    
    // re-enable interrupts
    sti();
}