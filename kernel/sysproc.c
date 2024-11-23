#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include <stddef.h> 


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




//shm

void *alloc_page(void)
{
    char *mem = kalloc();  // Assuming kalloc is a function that allocates kernel memory.
    if (mem == 0) {
        return 0;  // Return NULL if allocation fails
    }
    return mem;
}



#include "shm.h"

uint64
sys_shmget(void) {
    int key, size, shmflag;

    // Get arguments from user space
    if (argint(0, &key) < 0) return -1;
    if (argint(1, &size) < 0) return -1;
    if (argint(2, &shmflag) < 0) return -1;

    // Get the current process ID
    int pid = myproc()->pid;

    // Search for an existing shared memory segment with the same key
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        if (shm_segments[i].key == key) {
            // Increment reference count
            shm_segments[i].refcount++; 

            // Add the process ID to the list of processes using this segment
            for (int j = 0; j < MAX_PROCESSES_PER_SEGMENT; j++) {
                if (shm_segments[i].pids[j] == 0) {  // Find an empty spot for the PID
                    shm_segments[i].pids[j] = pid;
                    break;
                }
            }

            // Return the existing shared memory segment ID
            return shm_segments[i].id;
        }
    }

    // If no existing segment is found, create a new one
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        if (shm_segments[i].key == 0) {  // Unused slot
            shm_segments[i].key = key;
            shm_segments[i].size = size;
            shm_segments[i].shmflag = shmflag;
            shm_segments[i].addr = NULL;  // Allocate a page of memory for shared segment
            shm_segments[i].refcount = 1;
            shm_segments[i].id = i + 1;  // Assign a unique ID

            // Save the process ID to the new segment
            shm_segments[i].pids[0] = pid;  // Add the current process as the first process using this segment

            // Debugging statement
            printf("shmget: Created new segment with key %d, id %d\n", key, shm_segments[i].id);
            printf("shm_segments[%d].key = %d\n", i, shm_segments[i].key); // Print the key for the created segment
            return shm_segments[i].id;
        }
    }

    return -1;  // If no space for more segments
}


#define MAX_SHM_SEGMENTS 10  // Adjust based on the maximum number of shared segments




// Rename struct shm_segment to struct shmseg to match the declaration in shm.h
//struct shmseg shm_segments[MAX_SHM_SEGMENTS];  // Array to store shared memory segments




#define PGSIZE 4096  // Assuming 4KB pages


uint64
sys_shmat(void) {
    int shmid, shmflag;
    uint64 user_addr;  // Address provided by the user
    struct shmseg *segment;

    // Get system call arguments: shared memory ID, user address, and flags
    argaddr(1, &user_addr);
    if (argint(0, &shmid) < 0 ||  argint(2, &shmflag) < 0) {
        return -1;  // Invalid arguments
    }





    // Step 1: Validate the shared memory ID
    if (shmid < 1 || shmid > MAX_SHM_SEGMENTS || shm_segments[shmid-1].key == 0) {
        return -1;  // Invalid shared memory ID
    }
    segment = &shm_segments[shmid-1];
    int pid = myproc()->pid;  // Get the PID of the calling process
    int pid_found = 0;

    for (int i = 0; i < MAX_PROCESSES_PER_SEGMENT; i++) {
        if (segment->pids[i] == pid) {
            pid_found = 1;
            break;  // Process is already attached to this segment
        }
    }

    // If the PID is not found in the array, deny the request
    if (!pid_found) {
        printf("\nProcess does not have access, use shmget first ...\n");
        return -1;  // The process is not allowed to access this shared memory segment
    }
    // Step 2: Check if the segment already has an address
    if (segment->addr != NULL) {
        // If the segment has an address already, return that address to the user
        return (uint64)segment->addr;  // Return the address stored in the segment
    }
    printf("\nno prelocated address, assigning new one ...\n");

    // Step 3: Handle user-provided address if it's non-zero
    if (user_addr != 0) {
        // Ensure the user address is page-aligned
        // if (user_addr % PGSIZE != 0) {
        //     return -1;  // User address is not page-aligned
        // }

        // Store the user-provided address in shm_segments
    segment->addr = (void *)user_addr;
        segment->addr = (void *)user_addr;
        return (uint64)user_addr;
    } if (segment->addr == NULL) {
    // Allocate a page of memory for the shared memory segment (using alloc_page)
    void *allocated_addr = kalloc();
    if (allocated_addr == NULL) {
        return -1;  // Memory allocation failed
    }

    // Assign the allocated address to the segment's address field
    segment->addr = allocated_addr;

    // Use the allocated address for user address mapping
    user_addr = (uint64)(segment->addr);

    // Return the user address
    return user_addr;
}

    //return (uint64)user_addr;
    // Step 4: Map the shared memory segment to the user space if necessary
    struct proc *p = myproc();
    // Check if the address is already mapped or not, handle that case
    if (mappages(p->pagetable, user_addr, PGSIZE, (uint64)segment->addr, PTE_W | PTE_U) < 0) {
        return -1;  // Mapping failed
    }

    // Step 5: Increment the reference count for this shared memory segment
    segment->refcount++;

    // Debugging: Show the mapping info
    printf("shmat: Mapped shared memory segment id %d at user address: 0x%p\n", shmid, (void *)user_addr);

    // Step 6: Return the user-space virtual address
    return user_addr;  // The user can now access this address
}











uint64 sys_shmdt(void)
{
    int shmid;
    void *addr;

    // Get system call arguments
    if (argint(0, &shmid) < 0)
        return -1;
    if (argint(1, (int*)&addr) < 0)  // Using cast to void*
        return -1;

    // Find the shared memory segment by its ID (shmid)
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        if (shm_segments[i].id == shmid) {
            // Check if the address matches the segment's address
            printf("\nFound Id\n");
            //if (shm_segments[i].addr == addr) {
                // Step 1: Remove the PID from the pids array
                int pid = myproc()->pid;
                int pid_found = 0;

                // Search for the PID in the array and remove it
                for (int j = 0; j < MAX_PROCESSES_PER_SEGMENT; j++) {
                    if (shm_segments[i].pids[j] == pid) {
                        shm_segments[i].pids[j] = 0;  // Remove the PID
                        pid_found = 1;
                        break;
                    }
                }

                if (!pid_found) {
                    return -1;  // PID not found, should not happen if the caller is valid
                }

                // Step 2: Decrement the reference count
                shm_segments[i].refcount--;
                // if (shm_segments[i].refcount == 0) {
                //     // If no more references, we can clear the shared memory address and optionally free the memory
                //     //shm_segments[i].addr = NULL;
                // }

                // Shift remaining PIDs in the array (optional, to keep the array compact)
                for (int j = 0; j < MAX_PROCESSES_PER_SEGMENT - 1; j++) {
                    if (shm_segments[i].pids[j] == 0 && shm_segments[i].pids[j + 1] != 0) {
                        shm_segments[i].pids[j] = shm_segments[i].pids[j + 1];
                        shm_segments[i].pids[j + 1] = 0;
                    }
                }

                // Return success
                return 0;  // Successfully detached
            //}
        }
    }
    printf("\ndid not find the shmid\n");
    return -1;  // Shared memory segment not found or address mismatch
}



uint64 sys_shmctl(void) {
    int shmid, cmd, buf;

    // Get arguments from user space
    if (argint(0, &shmid) < 0) return -1;
    if (argint(1, &cmd) < 0) return -1;
    if (argint(2, &buf) < 0) return -1;

    // Validate shared memory ID
    if (shmid < 1 || shmid > MAX_SHM_SEGMENTS || shm_segments[shmid - 1].key == 0)
        return -1;

    switch (cmd) {
        case 1:  // Example: IPC_RMID (Remove the segment)
            shm_segments[shmid - 1].key = 0;
            return 0;
        case 2:  // Example: IPC_SET (Set segment properties)
            // You can modify other properties like shmflag here
            shm_segments[shmid - 1].shmflag = buf;
            return 0;
        default:
            return -1;
    }
}

