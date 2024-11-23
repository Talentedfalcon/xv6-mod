#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/param.h"
#include "kernel/semaphore.h"

struct semaphore sem;

void test_without_semaphore(){
    if(fork() == 0){
        for(int i=0;i<3;i++){
            printf("Child %d\n",i);
        }
        exit(0);
    }
    else{
        for(int i=0;i<3;i++){
            printf("Parent %d\n",i);
        }
        wait(0);
    }
}

void test_with_semaphore(){
    sem_init(&sem, 1);

    if(fork() == 0){
        sem_wait(&sem);
        for(int i=0;i<3;i++){
            printf("Child %d\n",i);
        }
        sem_post(&sem);
        exit(0);
    }
    else{
        sem_wait(&sem);
        for(int i=0;i<3;i++){
            printf("Parent %d\n",i);
        }
        sem_post(&sem);
        wait(0);
    }
}

int main(){
    printf("Wihtout Semaphore:\n");
    test_without_semaphore();
    printf("\nWith Semaphore:\n");
    test_with_semaphore();
    return 0;
}