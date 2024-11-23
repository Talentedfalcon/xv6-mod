// shm.h

#ifndef SHM_H
#define SHM_H
#define MAX_PROCESSES_PER_SEGMENT 10
#include "param.h"  // For maximum limits

// Define a shared memory segment structure
struct shmseg {
    int key;                // Key for shared memory segment
    int size;               // Size of the segment
    void *addr;             // Starting address of the segment
    int shmflag;            // Flags for shared memory (e.g., permissions)
    int refcount;           // Reference count for the number of processes using it
    int id;                 // Unique identifier for the segment
    int pids[MAX_PROCESSES_PER_SEGMENT];
};

// Maximum number of shared memory segments
#define MAX_SHM_SEGMENTS 10

extern struct shmseg shm_segments[MAX_SHM_SEGMENTS];
#define IPC_RMID 1    // Remove shared memory segment
#define IPC_SET  2    // Set shared memory properties
#define SHM_RDONLY 0x01  // Read-only flag
#define SHM_RND     0x02  // Round down flag (optional, depending on your use case)
#define SHM_EXEC    0x04  // Execute access flag (optional)

#endif
