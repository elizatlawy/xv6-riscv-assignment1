#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
//#include "kernel/syscall.h"


int main(int argc, char **argv) {
    // test new branch push
//    fprintf(1, "starting mask for: SYS_fork \n");
//    int mask = ((1 << SYS_fork));
//    trace(mask, 0);
//    trace(mask, 1);
//    trace(mask, 2);
    int pid = -1;
    for(int i = 0; i < 5; i++){
        if(pid != 0){
            fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perf;
        for(int i = 0; i < 10; i++){
            pid = wait_stat(&status, &perf);
            if (pid != -1){
                printf("child (%d) exited with status %d\n", pid, status);
                printf("creation time:    %d\n", perf.ctime);
                printf("termination time: %d\n", perf.ttime);
                printf("running time:     %d\n", perf.rutime);
                printf("runnable time:    %d\n", perf.retime);
                printf("sleeping time:    %d\n", perf.stime);
            }
            }
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d is running\n", my_pid);
        sleep(10000000);
        for ( int i = 0; i < (100000000 + (my_pid*100000000*10)); i++){}
        fprintf(2, "Child %d is finished runinng\n", my_pid);
    }
    exit(0);
}