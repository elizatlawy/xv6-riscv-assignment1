#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"


int main(int argc, char **argv) {
    // test new branch push
    fprintf(1, "starting mask for: SYS_fork \n");
    int mask = ((1 << SYS_fork) | (1 << SYS_kill) | (1 << SYS_sbrk) | (1 << SYS_exit) | (1 << SYS_wait));
    trace(mask, 0);
    trace(mask, 1);
    trace(mask, 2);
    int pid = fork();
    trace(mask, pid);
    if (pid != 0) { // parent
        int status;
        sleep(10);
        kill(pid);
        struct perf perf;
        perf.ctime = 0;
        perf.stime = 0;
        perf.ttime = 0;
        perf.retime = 0;
        perf.rutime = 0;
//        wait(&status);
        fprintf(2, "perf addr: %d\n", &perf);
        wait_stat(&status, &perf);
        fprintf(2, "ctime: %d\n", perf.ctime);
        fprintf(2, "stime: %d\n", perf.stime);
        fprintf(2, "ttime: %d\n", perf.ttime);
        fprintf(2, "retime: %d\n", perf.retime);
        fprintf(2, "rutime: %d\n", perf.rutime);
        fprintf(2, "Child %d finshed with exit status %d\n", pid, status);
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d is running\n", my_pid);
        sbrk(10);
        for ( int i = 0; i < 100; i++);
    }
    exit(0);
}