#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include "semaphore.h"

void sem_init(struct semaphore *sem, int value) {
  sem->value = value;
  initlock(&sem->lock, "semaphore");
  sem->head = sem->tail = 0;
}

void sem_wait(struct semaphore *sem) {
  acquire(&sem->lock);
  sem->value--;

  if (sem->value < 0) {
    // Add the current process to the waiting queue
    struct proc *p = myproc();
    sem->queue[sem->tail] = p;
    sem->tail = (sem->tail + 1) % NPROC;

    // Put the process to sleep
    sleep(p, &sem->lock);
  }

  release(&sem->lock);
}

void sem_post(struct semaphore *sem) {
  acquire(&sem->lock);
  sem->value++;

  if (sem->value <= 0) {
    // Wake up a process from the queue
    struct proc *p = sem->queue[sem->head];
    sem->head = (sem->head + 1) % NPROC;

    wakeup(p);
  }

  release(&sem->lock);
}
