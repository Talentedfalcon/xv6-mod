#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include "semaphore.h"

struct semaphore all_sem;

int sem_init(struct semaphore *sem, int value) {
  all_sem.value = value;
  initlock(&(all_sem.lock), "semaphore");
  all_sem.head = 0;
  all_sem.tail = 0;
  return 0;
}

int sem_wait(struct semaphore *sem) {
  acquire(&all_sem.lock);
  all_sem.value--;

  if (all_sem.value < 0){
    // Add the current process to the waiting queue
    struct proc *p = myproc();
    all_sem.queue[all_sem.tail] = p;
    all_sem.tail = (all_sem.tail + 1) % NPROC;

    // Put the process to sleep
    sleep(p, &all_sem.lock);
  }

  release(&all_sem.lock);
  return 0;
}

int sem_post(struct semaphore *sem) {
  acquire(&all_sem.lock);
  all_sem.value++;

  if (all_sem.value <= 0) {
    // Wake up a process from the queue
    struct proc *p = all_sem.queue[all_sem.head];
    all_sem.head = (all_sem.head + 1) % NPROC;

    wakeup(p);
  }

  release(&all_sem.lock);
  return 0;
}

int sem_destroy(struct semaphore *sem) {
    acquire(&all_sem.lock);

    if (all_sem.head!=all_sem.tail) {
        panic("sem_destroy: destroying semaphore with waiting processes");
    }

    all_sem.value = 0;
    for(int i=0;i<NPROC;i++){
      all_sem.queue[i]= 0;
    }

    release(&all_sem.lock);
    return 0;
}