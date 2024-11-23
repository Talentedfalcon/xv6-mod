#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "semaphore.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int 
sys_waitpid(void)
{
  int pid;
  uint64 status;
  int options;
  argint(0, &pid);
  argaddr(1, &status);
  argint(2, &options);
  return waitpid(pid, status, options);
}

int 
sys_sysinfo(void)
{
  uint64 kinfo; //pointer to sysinfo struct in user space
  argaddr(0, &kinfo);
  return sysinfo(kinfo);
}

uint64
sys_signal(void) {
    int signum;
    uint64 handler;
    // printf("\nIn sys_signal function\n");
    // Retrieve the signal number and the handler address
    argint(0, &signum);
    argaddr(1, &handler);
    // Only handle SIGINT for now
    if (signum != SIGINT)
        return -1;
    // Set the signal handler for the process
    struct proc *p = myproc();
    p->sig_handlers[SIGINT] = (void (*)(int))handler;
    return 0;
}
uint64
sys_sem_init(void){
  uint64 semptr;
  int value;
  argaddr(0,&semptr);
  argint(1,&value);
  
  struct semaphore ksem;

  if(copyin(myproc()->pagetable,(char*)&ksem,semptr,sizeof(ksem))<0)
    return -1;

  sem_init(&ksem,value);

  if(copyout(myproc()->pagetable,semptr,(char*)&ksem,sizeof(ksem))<0)
    return -1;

  return 0;
}

uint64
sys_sem_wait(void){
  uint64 semptr;
  argaddr(0,&semptr);

  struct semaphore ksem;

  if(copyin(myproc()->pagetable,(char*)&ksem,semptr,sizeof(ksem))<0)
    return -1;

  sem_wait(&ksem);

  if(copyout(myproc()->pagetable,semptr,(char*)&ksem,sizeof(ksem))<0)
    return -1;
  return 0;
}

uint64
sys_sem_post(void){
  uint64 semptr;
  argaddr(0,&semptr);

  struct semaphore ksem;

  if(copyin(myproc()->pagetable,(char*)&ksem,semptr,sizeof(ksem))<0)
    return -1;

  sem_post(&ksem);

  if(copyout(myproc()->pagetable,semptr,(char*)&ksem,sizeof(ksem))<0)
    return -1;
  return 0;
}