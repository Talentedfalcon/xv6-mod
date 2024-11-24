## New Features:
1. ### `sysinfo` System Call:
    `sysinfo` - return system information

    #### Function Prototype

    ```c 
    int sysinfo(struct sysinfo *info);
    ```

    #### Description

    The `sysinfo` system call provides detailed system statistics, including memory usage, uptime, and active process information.
    `sysinfo` returns the system information in a `struct sysinfo` variable, defined in `types.h`:

    ```c
    struct sysinfo {
        long uptime;           /* system uptime in seconds */
        uint64 freemem;        /* available memory size in bytes */
        uint64 totalmem;       /* total usable memory in bytes */
        uint64 procs;          /* number of running processes */
        uint64 pids[64];       /* pids of running processes */
    };
    ```

    #### Return Value

    On success `sysinfo()` returns `0`. On error, it returns `-1`.

2. ### `waitpid` System Call

    `waitpid` - wait for a specific child process to exit.

    #### Function Prototype

    ```c
    int waitpid(int pid, int *status, int options);
    ```

    #### Description

    The `waitpid()` system call allows a parent process to wait for a specific child process to exit. It can operate in blocking or non-blocking mode, depending on the options provided.
    It takes the following parameters:
    * `pid`: The Process ID (PID) of the child to wait for. Use -1 to wait for any child process.
    * `status`: An integer pointer to the variable in which child's exit status will be stored. Pass 0 if you do not need the exit status.
    * `options`: Controls the behavior of the system call:
        1. `WNOHANG` (non-zero): Return immediately if no child has exited.
        2. `WBLOCK` (0): Block until a child exits (default).

    #### Return Value

    `waitpid()` returns
    + `-1` if the calling process has no children, or the child does not exist, or the `copyout()` function fails.
    + `0` if the specified child exists but has not yet exited (only in non-blocking mode).
    + `pid` if the child has exited successfully.

3. ### `SIGINT` Signal: 
    - *Signal Handling*:  
        - Added support for *Ctrl+C (SIGINT)* detection through the terminal.  
        - Implemented a signal() system call for registering custom user-defined signal handlers.  

    - *Process Management*:  
        - Enhanced process structures to track pending signals and invoke handlers during user traps.  

    - *Console Integration*:  
        - Modified the console to recognize and trigger SIGINT upon *Ctrl+C* input.

4. ### `semaphore` System Calls:
    #### Function Prototype
    ```c
    int sem_init(struct semaphore*,int)
    ```
    ```c
    int sem_wait(struct semaphore*) 
    ```
    ```c
    int sem_post(struct semaphore*)
    ```
    ```c
    int sem_destroy(struct semaphore*)
    ```

    #### Description
    - `sem_init` initializes a semaphore object defined in `semaphore.h`:
        ```c
        struct semaphore {
            int value;                  // Semaphore value
            struct spinlock lock;       // Spinlock for atomic operations
            struct proc *queue[NPROC];  // Queue of waiting processes
            int head;                   // Queue indices for FIFO behavior
            int tail;
        };
        ```
    - `sem-wait` locks the semaphore by decrementing its value. If the value is zero, the calling process is blocked until the semaphore is available.
    - `sem-post` Unlocks the semaphore by incrementing its value. If other processes are blocked waiting on the semaphore, one is unblocked.
    - `sem-destroy` cleans up a semaphore's resources when it is no longer needed. It resets and deallocates associated data structures.

    #### Return Value
    All 3 semaphore functions return:
    - `0`: On success.
    - `-1`: On failure

5. ### `shm` System Calls:

## Structure Definition

The shared memory system in xv6 is implemented using the following structure:

```c
struct shmseg {
    int id;                              // Unique ID for the segment
    int key;                             // Key to identify the segment
    int size;                            // Size of the shared memory segment
    int shmflag;                         // Flags for segment permissions
    char *addr;                          // Kernel virtual address of the segment
    int refcount;                        // Reference count (number of processes using the segment)
    int pids[MAX_PROCESSES_PER_SEGMENT]; // List of process IDs using this segment
};
```

### Field Descriptions

- **id**: A unique identifier for each shared memory segment.
- **key**: A user-specified key to identify or create the shared memory segment.
- **size**: The size of the shared memory segment (typically a multiple of the page size, e.g., 4KB).
- **shmflag**: Flags that determine permissions and properties of the shared memory segment (e.g., read/write permissions).
- **addr**: The kernel virtual address of the allocated shared memory.
- **refcount**: The number of processes currently using this shared memory segment.
- **pids**: An array to keep track of process IDs of all processes using this segment.

### Constants

- **MAX_SHM_SEGMENTS**: Maximum number of shared memory segments that can be created.
- **MAX_PROCESSES_PER_SEGMENT**: Maximum number of processes that can attach to a shared memory segment.
- **PGSIZE**: The size of a memory page (typically 4KB).

## Key Features

1. **Shared Memory Creation (`sys_shmget`)**:
   - Creates a new shared memory segment if it doesn't exist or returns an existing segment if the key matches.
   - Allocates a kernel page for the memory and initializes metadata like `id`, `key`, `size`, and `refcount`.

2. **Attach to Shared Memory (`sys_shmat`)**:
   - Maps the shared memory to the user process's virtual address space.
   - Ensures proper alignment and handles address conflicts.

3. **Detach from Shared Memory (`sys_shmdt`)**:
   - Removes the calling process's association with the shared memory.
   - Decrements the `refcount` and removes the process ID from the list.

4. **Control Shared Memory (`sys_shmctl`)**:
   - Provides administrative operations like removing a shared memory segment (`IPC_RMID`) or updating its properties (`IPC_SET`).

## Usage Example

```c
// Example usage of shared memory system calls
int shmid = shmget(1234, PGSIZE, 0);  // Create or get shared memory with key 1234
void *addr = shmat(shmid, 0, 0);      // Attach to the shared memory
shmdt(addr);                          // Detach from the shared memory
shmctl(shmid, IPC_RMID, 0);           // Remove the shared memory segment
```

## Debugging and Validation

- **Debug Messages**:
  - `shmat`: Logs the mapping of shared memory to a process's address space.
  - `shmdt`: Logs the detachment and reference count updates.
  - `shmget`: Logs creation of new segments and reuse of existing ones.

- **Validation**:
  - Ensures virtual address alignment and availability during `shmat`.
  - Confirms that a shared memory ID is valid before performing operations.
  - Verifies page table entries (PTE) for correct permissions.

## References

- **xv6 Book**: Chapter on memory management.
- **POSIX Shared Memory**: Concepts of `shmget`, `shmat`, `shmdt`, and `shmctl` in UNIX systems.
