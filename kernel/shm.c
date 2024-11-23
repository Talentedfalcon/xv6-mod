// shm.c or sysproc.c
#include "shm.h"
#include<stdio.h>

// Array to store the shared memory segments
struct shmseg shm_segments[MAX_SHM_SEGMENTS];

// Initialize shared memory segments (in kernel initialization)
void init_shm(void) {
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        shm_segments[i].key = 0;  // Not in use
        shm_segments[i].size = 0;
        shm_segments[i].addr = NULL;
        shm_segments[i].shmflag = 0;
        shm_segments[i].refcount = 0;
        shm_segments[i].id = 0;
    }
}
