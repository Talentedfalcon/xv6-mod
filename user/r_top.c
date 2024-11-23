//r: custom implementation of top program
//uses the sysinfo system call to get system information

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    struct sysinfo info;
    int ret = sysinfo(&info);
    if(ret < 0) {
        printf("sysinfo failed\n");
        exit(1);
    }

    //make it nice like htop
    printf("rtop - ");
    printf("system uptime: %d seconds ", (int)info.uptime);
    printf("\tmemory usage: %ld %%\n",(info.totalmem - info.freemem)*100/info.totalmem);
    printf("total memory: %ld KiB", info.totalmem/1024);
    printf("\t\tfree memory: %ld KiB\n", info.freemem/1024);
    printf("processes: %ld\n", info.procs);
    printf("PID\t STATE\n");
    for(int i = 0; i < info.procs; i++) {
        printf("%ld\t %s\n", info.pids[i], "RUNNING");
    }

    return 0;
}