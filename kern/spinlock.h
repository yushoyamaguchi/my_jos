// Mutual exclusion lock.
struct spinlock {
  int locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  int n;
  int nts;
};