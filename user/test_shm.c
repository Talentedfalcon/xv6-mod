#include "user.h"
#include "kernel/shm.h"

#define SHM_KEY 1234
#define SHM_SIZE 4096
#define IPC_CREAT 0x01000
#define IPC_RMID  1

int main() {
    printf("Shared Memory Test Program\n");
    int check = fork();
    char str[100];
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    char* shm_addr =  (char*)shmat(shmid,&str, 0);
    for (int i = 0; i < 26; i++) {
        str[i] = 'A' + i;  // Write A-Z into shared memory
    }
    str[26] = '\0';
    // Step 1: Create or get a shared memory segment
    if(check==0){
    if (shmid < 0) {
        printf("shmget failed\n");
        exit(-1);
    }
    printf("shmget: Created shared memory segment with ID: %d\n", shmid);

    shm_addr = (char*)shmat(shmid,&str, 0);
    if (shm_addr == (char*)0) {
        printf("shmat failed\n");
        exit(-1);
    }
    printf("shmat: Attached to shared memory at address: %p\n", shm_addr);

    // Step 3: Write data to shared memory
    printf("Writing to shared memory... %s()()\n",shm_addr);
    for (int i = 0; i < 26; i++) {
        shm_addr[i] = 'A' + i;  // Write A-Z into shared memory
    }
    shm_addr[26] = '\0';  // Null-terminate the string
    printf("Shared memory content: %s\n", shm_addr);

    //Step 4: Detach from the shared memory
    if (shmdt(shmid,shm_addr) < 0) {
        printf("shmdt failed\n");
        exit(-1);
    }
    printf("shmdt: Detached from shared memory\n");
    }else{
        wait(0);
    //int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    // // Step 5: Reattach to verify content
    char* shm_addr2 = (char*)shmat(shmid, &str, 0);
    if (shm_addr2 == (char*)0) {
        printf("shmat (reattach) failed\n");
        exit(-1);
    }
    printf("shmat: Reattached to shared memory at address: %p\n", shm_addr2);
    printf("Shared memory content after reattachment: %s\n", shm_addr);

    // // Step 6: Clean up (remove the shared memory segment)
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("shmctl (remove) failed\n");
        exit(-1);
    }
    printf("shmctl: Removed shared memory segment\n");
    }
    return 0;
}
