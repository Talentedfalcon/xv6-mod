#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid1, pid2;
    int status;

    // Create the first child process
    if ((pid1 = fork()) == 0) {
        
        printf("Child 1 (pid: %d) starting.\n", getpid());
        sleep(10);  // Simulate work
        printf("Child 1 (pid: %d) exiting.\n", getpid());
        exit(1);
    }

    // Create the second child process
    if ((pid2 = fork()) == 0) {
        sleep(10);
        printf("Child 2 (pid: %d) starting.\n", getpid());
        sleep(10);  // Simulate longer work
        printf("Child 2 (pid: %d) exiting.\n", getpid());
        exit(2);
    }

    // Test waitpid with blocking (flag = 0)
    int waited_pid = waitpid(pid1, &status, WBLOCK);  // Blocking mode
    if (waited_pid > 0) {
        printf("Parent: waitpid (blocking) -> Child with pid %d exited with status %d.\n", waited_pid, status);
    } else {
        printf("Parent: waitpid (blocking) -> No child found.\n");
    }

    // Test waitpid with non-blocking (flag = 1)
    waited_pid = waitpid(pid2, &status, WNOHANG);  // Non-blocking mode
    if (waited_pid == 0) {
        printf("Parent: waitpid (non-blocking) -> Child with pid %d is still running.\n", pid2);
    } else if (waited_pid > 0) {
        printf("Parent: waitpid (non-blocking) -> Child with pid %d exited with status %d.\n", waited_pid, status);
    } else {
        printf("Parent: waitpid (non-blocking) -> No child found.\n");
    }

    // Ensure no more children are present
    waited_pid = waitpid(1,&status,WNOHANG);
    if (waited_pid < 0) {
        printf("Parent: No more children to wait for.\n");
    } 

    exit(0);
}
