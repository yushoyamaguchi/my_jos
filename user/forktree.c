// Fork a binary tree of processes and display their structure.

#include <inc/lib.h>

#define DEPTH 3

void forktree(const char *cur);

void
forkchild(const char *cur, char branch)
{
	char nxt[DEPTH+1];

	if (strlen(cur) >= DEPTH)
		return;

	snprintf(nxt, DEPTH+1, "%s%c", cur, branch);
	if (fork() == 0) {
		forktree(nxt);
		exit();
	}
}

void
forktree(const char *cur)
{
	cprintf("%04x: start : I am '%s'\n", sys_getenvid(), cur);

	forkchild(cur, '0');
	forkchild(cur, '1');
	long i;
	long big = 100000000;
	for (i = 1; i <= big; i++){
		if(i % big == 0){
			cprintf("%04x: end : I am '%s'\n", sys_getenvid(), cur);
		}
	}
}

void
umain(int argc, char **argv)
{
	forktree("");
}

