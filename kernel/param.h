#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name
#define QUANTUM      5   // time measured as 5 clock ticks - for ass1, Task3
#define ALPHA        50   // alpha parameter for srt scheduler
#define TESTHIGH     1  // decay factor of priority 1
#define HIGH         3 // decay factor of priority 2
#define NORMAL       5    // decay factor of priority 3
#define LOW          7  // decay factor of priority 4
#define TESTLOW      25  // decay factor of priority 5