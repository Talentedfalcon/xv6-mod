#include "kernel/types.h"
#include "user/user.h"
// #include "kernel/semaphore.h"

struct semaphore sem;

void test_semaphore() {
    sem_init(&sem, 1); // Binary semaphore

    if (fork() == 0) {
        // Child process
        sem_wait(&sem);
        printf("Child acquired semaphore\n");
        sem_post(&sem);
        exit(0);
    } else {
        // Parent process
        sem_wait(&sem);
        printf("Parent acquired semaphore\n");
        sem_post(&sem);
        wait(0);
    }
}

int main(){
    test_semaphore();
}