typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

struct sysinfo {
    long uptime;
    uint64 freemem;
    uint64 totalmem;
    uint64 procs;
    uint64 pids[64];
};
