#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"

void srt_test();

void trace_tests();
void fcfs_test();

void cfsd_test();


int main(int argc, char **argv) {
    srt_test();
    exit(0);
}

void trace_tests() {
    int mask = ((1 << SYS_fork) | (1 << SYS_kill) | (1 << SYS_sbrk) | (1 << SYS_write));
    sleep(1); //doesn't print this sleep
    trace(mask, getpid());
    int cpid = fork();//prints fork once
    if (cpid == 0) {
        fork();// prints fork for the second time - the first son forks
        sbrk(1);
//        mask= (1<< 13); //to turn on only the sleep bit
        //mask= (1<< 1)|(1<< 13); you can uncomment this inorder to check you print for both fork and sleep syscalls
        trace(mask, getpid()); //the first son and the grandchilde changes mask to print sleep
        sleep(1);
        fork();//should print nothing
        exit(0);//shold print nothing
    } else {
        sleep(10);// the father doesnt pring it - has original mask
    }
    exit(0);
}


void srt_test() {
//    set_priority(2);
    int pid = -1;
    for (int i = 0; i < 10; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perf;
        for (int i = 0; i < 10; i++) {
            pid = wait_stat(&status, &perf);
            if (pid != -1) {
                printf("child (%d) exited with status %d\n", pid, status);
                printf("creation time:    %d\n", perf.ctime);
                printf("termination time: %d\n", perf.ttime);
                printf("running time:     %d\n", perf.rutime);
                printf("runnable time:    %d\n", perf.retime);
                printf("sleeping time:    %d\n", perf.stime);
                printf("average_bursttime:    %d\n", perf.bursttime);
            }
        }
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d is running\n", my_pid);
        int iter = getpid();
        while (iter > 0) { // 10 iter of for + sleep.
            for (int i = 0; i < (1000000000 * my_pid); i++) {
            }
            sleep(my_pid);
            for (int i = 0; i < (1000000000 * my_pid); i++) {
            }
            iter--;
        }
        fprintf(2, "Child %d is finished runinng\n", my_pid);
    }
}


void cfsd_test(){
    set_priority(1);
    int pid = -1;
    int priority = 5;
    for (int i = 0; i < 10; i++) {
        if (pid != 0) {
            pid = fork();
            // set child priority 5 to 1 over & over
            priority --;
            if( priority < 1)
                priority = 5;
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perf;
        for (int i = 0; i < 10; i++) {
            pid = wait_stat(&status, &perf);
            if (pid != -1) {
                printf("child (%d) exited with status %d\n", pid, status);
                printf("creation time:    %d\n", perf.ctime);
                printf("termination time: %d\n", perf.ttime);
                printf("running time:     %d\n", perf.rutime);
                printf("runnable time:    %d\n", perf.retime);
                printf("sleeping time:    %d\n", perf.stime);
                printf("average_bursttime:    %d\n", perf.bursttime);
            }
        }
    } else { // child
        set_priority(priority);
        int my_pid = getpid();
        fprintf(2, "Child %d is running\n", my_pid);
        int iter = my_pid;
        while (iter > 0) { // 10 iter of for + sleep.
            for (int i = 0; i < (100 * my_pid); i++) {
            }
            sleep(10);
            for (int i = 0; i < (100 * my_pid); i++) {
            }
            iter--;
        }
        fprintf(2, "Child %d is finished runinng\n", my_pid);
    }

}

void fcfs_test() {
    int pid = -1;
    for (int i = 0; i < 10; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perf;
        for (int i = 0; i < 10; i++) {
            pid = wait_stat(&status, &perf);
            if (pid != -1) {
//                printf("child (%d) exited with status %d\n", pid, status);
//                printf("creation time:    %d\n", perf.ctime);
//                printf("termination time: %d\n", perf.ttime);
//                printf("running time:     %d\n", perf.rutime);
//                printf("runnable time:    %d\n", perf.retime);
//                printf("sleeping time:    %d\n", perf.stime);
            }
        }
    } else { // child
//        int my_pid = getpid();
//        fprintf(2, "Child %d is running\n", my_pid);
        for (int i = 0; i < (1000 * getpid()); i++) {
        }
//        sleep(10*getpid());
        for (int i = 0; i < (1000 * getpid()); i++) {
        }
        //       fprintf(2, "Child %d is finished runinng\n", my_pid);
    }
}
