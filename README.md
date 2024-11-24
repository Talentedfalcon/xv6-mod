## New Features:
1. ### `sysinfo` System Call:
    `sysinfo` - return system information

    **Function Prototype**

    ```c 
    int sysinfo(struct sysinfo *info);
    ```

    **Description**

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

    **Return Value**

    On success `sysinfo()` returns `0`. On error, it returns `-1`.

2. ### `waitpid` System Call

    `waitpid` - wait for a specific child process to exit.

    **Function Prototype**

    ```c
    int waitpid(int pid, int *status, int options);
    ```

    **Description**

    The `waitpid()` system call allows a parent process to wait for a specific child process to exit. It can operate in blocking or non-blocking mode, depending on the options provided.
    It takes the following parameters:
    * `pid`: The Process ID (PID) of the child to wait for. Use -1 to wait for any child process.
    * `status`: An integer pointer to the variable in which child's exit status will be stored. Pass 0 if you do not need the exit status.
    * `options`: Controls the behavior of the system call:
        1. `WNOHANG` (non-zero): Return immediately if no child has exited.
        2. `WBLOCK` (0): Block until a child exits (default).

    **Return Value**

    `waitpid()` returns
    + `-1` if the calling process has no children, or the child does not exist, or the `copyout()` function fails.
    + `0` if the specified child exists but has not yet exited (only in non-blocking mode).
    + `pid` if the child has exited successfully.

3. ### `SIGINT` Signal: 
    - **Signal Handling**:  
        - Added support for *Ctrl+C (SIGINT)* detection through the terminal.  
        - Implemented a signal() system call for registering custom user-defined signal handlers.  

    - **Process Management**:  
        - Enhanced process structures to track pending signals and invoke handlers during user traps.  

    - **Console Integration**:  
        - Modified the console to recognize and trigger SIGINT upon *Ctrl+C* input.

4. ### `semaphore` System Calls:
    **Function Prototype**
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

    **Description**
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

    **Return Value**
    All 4 semaphore functions return:
    - `0`: On success
    - `-1`: On failure

5. ### `sys_shmget` System Call

    `sys_shmget` - Allocate or retrieve a shared memory segment.

    **Function Prototype**

    ```c
    uint64 sys_shmget(void);
    ```

    **Description**

    The `sys_shmget` system call allocates or retrieves a shared memory segment. It takes three arguments:
    - `key`: A unique identifier for the shared memory segment.
    - `size`: The size of the shared memory segment.
    - `shmflag`: Flags controlling segment behavior.

        If a segment with the same key already exists, it increments the reference count and returns the existing segment ID. If the segment does not exist, it creates a new one, allocates memory for it, and returns the new segment ID.

    **Return Value**
    On success, `sys_shmget` returns the shared memory segment ID. On failure, it returns `-1`.

6. ### `sys_shmat` System Call

    `sys_shmat` - Attach a shared memory segment to a process's address space.

    **Function Prototype**

    ```c
    uint64 sys_shmat(void);
    ```

    **Description**

    The `sys_shmat` system call attaches a shared memory segment to the calling process's address space. It takes the shared memory ID (`shmid`) and the user space address (`user_addr`) where the segment should be attached.

    - If the `user_addr` is not provided or is 0, the system finds a free address space for the attachment.
    - If the `user_addr` is provided, it must be page-aligned.
    - The function maps the shared memory segment's physical address into the process's virtual address space.
        
    **Return Value**

    On success, `sys_shmat` returns the virtual address where the shared memory segment is mapped. On failure, it returns `-1`.

7. ### `sys_shmdt` System Call

    `sys_shmdt` - Detach a shared memory segment from a process's address space.

    **Function Prototype**

    ```c
    uint64 sys_shmdt(void);
    ```

    **Description**

    The `sys_shmdt` system call detaches a shared memory segment from the calling process's address space. It takes the shared memory ID (`shmid`) and the address to be detached (`addr`).

    - The process's PID is removed from the segment's process list.
    - The reference count for the segment is decremented.

    **Return Value**

    On success, `sys_shmdt` returns `0`. On failure, it returns `-1`.

8. ### `sys_shmctl` System Call

    `sys_shmctl` - Control shared memory segments.

    **Function Prototype**

    ```c
    uint64 sys_shmctl(void);
    ```

    **Description**

    The `sys_shmctl` system call performs various control operations on shared memory segments. It takes three arguments:
    - `shmid`: The shared memory segment ID.
    - `cmd`: The control command (e.g., remove or modify flags).
    - `buf`: A buffer holding additional data (depending on the command).

    Some possible commands:
        - `IPC_RMID`: Removes the shared memory segment.
        - `IPC_SET`: Modifies the segment's properties.

   **Return Value**

    On success, `sys_shmctl` returns `0`. On failure, it returns `-1`.

9. ### `alloc_page` Function

    `alloc_page` - Allocate a page of memory.

    **Function Prototype**

    ```c
    void* alloc_page(void);
    ```

    **Description**

    The `alloc_page` function allocates a single page of memory using the kernel's memory allocator (`kalloc`). If the allocation fails, it returns `NULL`.

    **Return Value**

    On success, `alloc_page` returns a pointer to the allocated memory page. On failure, it returns `NULL`.

10. ### `clear` Command:
    - Clears the terminal screen

## Testing the New Features:
1. ### sys_info test:
    ![image](https://github.com/user-attachments/assets/49ee6abd-5a39-449d-b3f0-45f6d6483340)
2. ### waitpid test:
   ![image](https://github.com/user-attachments/assets/061ee7d7-e553-427d-a1a4-3a4d751417cb)
3. ### SIGINT test:
   ![image](https://github.com/user-attachments/assets/73e0be86-c5bf-4f9a-81e5-e495afc338dc)
4. ### semaphore test:
   ![image](https://github.com/user-attachments/assets/2d37158d-43f4-4fe2-89aa-065298227d85)
5. ### shm test:
   ![image](https://github.com/user-attachments/assets/b2d0c0c5-2588-4b4f-af16-7beee1322c3b)


