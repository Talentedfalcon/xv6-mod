# `sysinfo` System Call

`sysinfo` - return system information

## Function Prototype

```c
#include "user.h"
    
int sysinfo(struct sysinfo *info);
```

## Description

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

## Return Value

On success `sysinfo()` returns `0`. On error, it returns `-1`.

# `waitpid` System Call

`waitpid` - wait for a specific child process to exit.

## Function Prototype

```c
#include "user.h"

int waitpid(int pid, int *status, int options);
```

## Description

The `waitpid()` system call allows a parent process to wait for a specific child process to exit. It can operate in blocking or non-blocking mode, depending on the options provided.
It takes the following parameters:
* `pid`: The Process ID (PID) of the child to wait for. Use -1 to wait for any child process.
* `status`: An integer pointer child's exit status will be stored. Pass 0 if you do not need the exit status.
* `options`: Controls the behavior of the system call:
    1. WNOHANG (non-zero): Return immediately if no child has exited.
    2. WBLOCK (0): Block until a child exits (default).

## Return Value

`waitpid()` returns,
+ `-1` if the calling process has no children, or the child does not exist, or the `copyout()` function fails.
+ `0` if the specified child exists but has not yet exited (only in non-blocking mode).
+ `pid` if the child has exited successfully.
