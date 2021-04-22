#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void) {
    int n;
    if (argint(0, &n) < 0)
        return -1;
    exit(n);
    return 0;  // not reached
}

uint64
sys_getpid(void) {
    return myproc()->pid;
}

uint64
sys_fork(void) {
    return fork();
}

uint64 sys_wait(void) {
    uint64 p;
    if (argaddr(0, &p) < 0)
        return -1;
    return wait(p);
}

uint64 sys_wait_stat(void) {
    uint64 status;
    uint64 performance;
    int pid;
    if (argaddr(0, &status) < 0)
        return -1;
    if (argaddr(1, &performance) < 0)
        return -1;
    pid = wait_stat(status, (struct perf *) performance);
    return pid;
}

uint64 sys_sbrk(void) {
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;
    addr = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

uint64
sys_sleep(void) {
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

uint64
sys_kill(void) {
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void) {
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}

// ass1 - trace syscall
uint64 sys_trace(void) {
    int mask;
    if (argint(0, &mask) < 0) {
        return -1; // illegal arg
    }
    int pid;
    if (argint(1, &pid) < 0) {
        return -1; // illegal arg
    }
    return trace(mask,pid);
}
uint64 sys_set_priority(void) {
    int decay_fact_arr[] = { // 0 is just empty space for convenience
            0, TESTHIGH, HIGH, NORMAL, LOW, TESTLOW
    };
    int priority;
    if (argint(0, &priority) < 0) {
        return -1;
    }
    if (priority < 1 || priority > 5)
        return -1;
    myproc()->decay_factor = decay_fact_arr[priority];
    return 0;
}

// yield system call - just for testing
uint64 sys_yield(void){
    yield();
    return 0;
}

// get tick system call - just for testing
uint64 sys_get_ticks(void){
    return ticks;
}

uint64 sys_change_bursttime(void){
    int new_bursttime;
    if (argint(0, &new_bursttime) < 0)
        return -1;
    myproc()->perf.average_bursttime = new_bursttime;
    return 0;
}
