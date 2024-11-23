#include "user.h"
#include "kernel/shm.h"

#define SHM_KEY 1234
#define SHM_SIZE 4096
#define IPC_CREAT 0x01000
#define IPC_RMID  1

int main() {
    printf("Shared Memory Test Program 2\n");
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    // // Step 5: Reattach to verify content
    char* shm_addr2 = (char*)shmat(shmid, 0, 0);
    if (shm_addr2 == (char*)0) {
        printf("shmat (reattach) failed\n");
        exit(-1);
    }
    printf("shmat: Reattached to shared memory at address: %p\n", shm_addr2);
    printf("Shared memory content after reattachment: %s\n", shm_addr2);

    // // Step 6: Clean up (remove the shared memory segment)
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("shmctl (remove) failed\n");
        exit(-1);
    }
    printf("shmctl: Removed shared memory segment\n");

    return 0;
}
