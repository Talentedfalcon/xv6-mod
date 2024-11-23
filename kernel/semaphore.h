struct semaphore {
  int value;              // Semaphore value
  struct spinlock lock;   // Spinlock for atomic operations
  struct proc *queue[NPROC]; // Queue of waiting processes
  int head; 
  int tail;         // Queue indices for FIFO behavior
};
