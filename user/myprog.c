#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"

void srt_test1();

void srt_test2();

void srt_tets3();

void trace_tests();

void fcfs_test1();

void cfsd_tests();

int main(int argc, char **argv) {
    srt_tets3();
//    srt_tets3();
//    fcfs_test1();
//    srt_test1();
    exit(0);
}

//// this test create 3 child process: first set burst time to: 50000 , second to 100, third to 5000 and all yield
//// expected order of finish: pid 5 -> pid 6 -> pid 4
void srt_test2(){
    int pid = -1;
    for (int i = 0; i < 3; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perfs[10];
        int pids[10];
        for (int i = 0; i < 3; i++) {
            pids[i] = wait_stat(&status, &perfs[i]);
        }
        // print all child's pref only after everyone is finished just to keep the printing in order.
        for (int i = 0; i < 3; i++) {
            printf("child (%d) exited with status %d\n", pids[i], status);
            printf("creation time:    %d\n", perfs[i].ctime);
            printf("termination time: %d\n", perfs[i].ttime);
            printf("running time:     %d\n", perfs[i].rutime);
            printf("runnable time:    %d\n", perfs[i].retime);
            printf("sleeping time:    %d\n", perfs[i].stime);
            printf("average_bursttime:    %d\n", perfs[i].average_bursttime);
        }
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d started running\n", my_pid);
        int start = get_ticks();
        int end = 0;
        if (my_pid == 4) // first child
            change_bursttime(50000);
        else if (my_pid == 5)// second child
            change_bursttime(100);
        else if (my_pid == 6)  // third child
            change_bursttime(5000);
        yield();
        end = start + 20; // all child will run for 20 ticks
        while (1){ // while true
            if (get_ticks() > end)  // run while loop until reach the correct tick
                break;
        }
    }
}

//// this test create 3 child process: first run for 24 ticks, second for 10 ticks, third for 1 tick
//// expected order of finish: pid 4 -> pid 5 -> pid 6
void fcfs_test1() {
    int pid = -1;
    for (int i = 0; i < 3; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perfs[10];
        int pids[10];
        for (int i = 0; i < 3; i++) {
            pids[i] = wait_stat(&status, &perfs[i]);
        }
        // print all child's pref only after everyone is finished just to keep the printing in order.
        for (int i = 0; i < 3; i++) {
            printf("child (%d) exited with status %d\n", pids[i], status);
            printf("creation time:    %d\n", perfs[i].ctime);
            printf("termination time: %d\n", perfs[i].ttime);
            printf("running time:     %d\n", perfs[i].rutime);
            printf("runnable time:    %d\n", perfs[i].retime);
            printf("sleeping time:    %d\n", perfs[i].stime);
            printf("average_bursttime:    %d\n", perfs[i].average_bursttime);
        }
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d started running\n", my_pid);
        int start = get_ticks();
        int end = 0;
        if (my_pid == 4) // first child
            end = start + 24;
        else if (my_pid == 5)// second child
            end = start + 10;
        else  // third child
            end = start + 1;
        while (1){ // while true
            if (get_ticks() > end) // run while loop until reach the correct tick
                break;
        }
    }
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


void srt_test1() {
    int pid = -1;
    for (int i = 0; i < 2; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perf;
        for (int i = 0; i < 10; i++) {
            pid = wait_stat(&status, &perf);
                printf("terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                       pid, perf.ctime, perf.ttime, perf.stime,
                       perf.retime, perf.rutime, perf.average_bursttime);
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
//        fprintf(2, "Child %d is finished runinng\n", my_pid);
    }
}

/// test form Yaeli not mine!
void srt_tets3(){
    fprintf(2, "Hello world!\n");
    struct perf performance;
    int mask=(1<< 1) | (1<< 23) | (1<< 3);
    trace(mask, getpid());
    int cpid = fork();
    if (cpid != 0){
        int t_pid = wait_stat(0, &performance);
        fprintf(1, "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                t_pid, performance.ctime, performance.ttime, performance.stime,
                performance.retime, performance.rutime, performance.average_bursttime);
    }
    else{
        sleep(10);
        for(int i=1; i < 15; i++){
            int c_pid = fork();
            if(c_pid == 0){ // Child
                sleep(i);
                exit(0);
            }
            else{
                int i = 0;
                while(i<100000000){
                    i++;
                }
            }
        }
        sleep(10);
        for(int i=1; i < 15; i++){
            int c_pid = fork();
            if(c_pid == 0){
                int i = 0;
                while(i<10000000){
                    i++;
                }
                exit(0);
            }
            else{
                int t_pid = wait_stat(0, &performance);
                fprintf(1, "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                        t_pid, performance.ctime, performance.ttime, performance.stime, performance.retime,
                        performance.rutime, performance.average_bursttime);
                int i = 0;
                while(i<10000){
                    i++;
                }
            }
        }
        int t_pid = wait_stat(0, &performance);
        fprintf(1, "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                t_pid, performance.ctime, performance.ttime, performance.stime, performance.retime,
                performance.rutime, performance.average_bursttime);
        exit(0);
    }
}
void cfsd_test1() {
    set_priority(1);
    int pid = -1;
    int priority = 5;
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
                printf("average_bursttime:    %d\n", perf.average_bursttime);
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



