#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"

void srt_test();
void trace_tests();

int main(int argc, char **argv) {
    set_priority(2);
    int pid = -1;
    pid = fork();
    if(pid == 0){
        exec("ls",argv);
    }
    exit(0);
}

void trace_tests(){
    int mask=((1<< SYS_fork)|( 1<< SYS_kill)| ( 1<< SYS_sbrk) | ( 1<< SYS_write));
    sleep(1); //doesn't print this sleep
    trace(mask, getpid());
    int cpid=fork();//prints fork once
    if (cpid==0){
        fork();// prints fork for the second time - the first son forks
        mask= (1<< 13); //to turn on only the sleep bit
        //mask= (1<< 1)|(1<< 13); you can uncomment this inorder to check you print for both fork and sleep syscalls
        trace(mask, getpid()); //the first son and the grandchilde changes mask to print sleep
        sleep(1);
        fork();//should print nothing
        exit(0);//shold print nothing
    }
    else {
        sleep(10);// the father doesnt pring it - has original mask
    }
    exit(0);
}

//example for right printing:
//
//3: syscall fork 0-> 4
//4: syscall fork 0-> 5
//4: syscall sleep -> 0
//5: syscall sleep -> 0

void srt_test(){
//    set_priority(2);
    int pid = -1;
    for(int i = 0; i < 10; i++){
        if(pid != 0){
            pid = fork();
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
        for(int i=0;i<(100*getpid());i++){
        }
        sleep(10);
        for(int i=0;i<(100*getpid());i++) {
        }
        fprintf(2, "Child %d is finished runinng\n", my_pid);
    }
    exit(0);
}
