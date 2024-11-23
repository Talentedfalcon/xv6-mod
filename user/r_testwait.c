//r: program to test waitpid system call

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int pid1, pid2, pid3;
    int status;

    pid1 = fork();
    if(pid1 == 0) {
        printf("child 1 pid: %d\n", getpid());
        sleep(10);
        exit(1);
    }

    pid2 = fork();
    if(pid2 == 0) {
        sleep(1);
        printf("child 2 pid: %d\n", getpid());
        sleep(200);
        exit(2);
    }

    pid3 = fork();
    if(pid3 == 0) {
        sleep(2);
        printf("child 3 pid: %d\n", getpid());
        exit(3);
    }

// blocking wait
    if(waitpid(pid1, &status, WBLOCK) != -1) //wait for child 1 to exit
        printf("child 1 exited with status %d\n", status);
    else printf("child 1 does not exist\n");

// non-blocking wait, child 2 not finished
    int retval = waitpid(pid2, &status, WNOHANG);
    
    if(retval == 0) 
        printf("child 2 has NOT yet exited\n");
    else if(retval != -1) //wait for child 2 to exit
        printf("child 2 exited with status %d\n", status);
    else 
        printf("child 2 does not exist\n");

// non-blocking wait, but child 3 finished
    if(waitpid(pid3, &status, WNOHANG) != -1) //wait for child 3 to exit
        printf("child 3 exited with status %d\n", status);
    else if(status == 0) 
        printf("child 3 has not exited\n");
    else 
        printf("child 3 does not exist\n");

    
    return 0;
}