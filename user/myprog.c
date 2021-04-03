#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"

void srt_test1();

void srt_test2();

void srt_tets3();

void trace_tests();

void trace_all_sys_calls_tests();


void fcfs_test1();

void fcfs_test2();

void cfsd_test1();

void test_scheduler();

void sanity_tests();

void sanity_tests2();

#define NUM_OF_CHILDS 100
#define CLAC_MEDIUM_SIZE_LOOP 10000000
#define CALC_LARGE_SIZE_LOOP 2000000000
#define IO_MEDIUM_SIZE_LOOP 100000
#define IO_LARGE_SIZE_LOOP 2000000

int gcd(int a, int b) {
    int c;
    while (a != 0) {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}


void small_loop_calculation() {
    for (int j = 0; j < CLAC_MEDIUM_SIZE_LOOP; ++j)
        gcd(j + 1, j + 2);

}


void large_loop_calculation() {
    for (int j = 0; j < CALC_LARGE_SIZE_LOOP; ++j)
        gcd(j + 1, j + 2);

}


void small_loop_print() {
    for (int j = 0; j < IO_MEDIUM_SIZE_LOOP; ++j) {
        printf("hello world!\n");
    }
}

void large_loop_print() {
    for (int j = 0; j < IO_LARGE_SIZE_LOOP; ++j) {
        printf("hello world!\n");
    }
}

void reparent();

int main(int argc, char **argv) {
    reparent();
//    fcfs_test1();
//    sanity_tests2();
//    test_scheduler();
//    fcfs_test2();
//    cfsd_test1();
//    srt_tets3();
//    srt_tets3();
//    srt_test1();
//    trace_all_sys_calls_tests();
    exit(0);
}

void reparent()
{
    int master_pid = getpid();
    for(int i = 0; i < 200; i++){
        int pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            exit(1);
        }
        if(pid){
            if(wait(0) != pid){
                printf("wait wrong pid\n");
                exit(1);
            }
        } else {
            int pid2 = fork();
            if(pid2 < 0){
                kill(master_pid);
                exit(1);
            }
            exit(0);
        }
    }
    exit(0);
}

void trace_all_sys_calls_tests(){
    int mask = ((1 << SYS_fork) | (1 << SYS_kill) | (1 << SYS_sbrk)  | ( 1 << SYS_exit) |
            (1 <<SYS_sleep  )  | (1 << SYS_pipe ) | (1 << SYS_kill) | (1 << SYS_exec) | (1 << SYS_read) | (1 << SYS_chdir) | (1 << SYS_getpid)
            | (1 << SYS_yield) | (1 << SYS_mkdir) | (1 << SYS_wait_stat) | (1 << SYS_set_priority));
    sleep(1); //doesn't print this sleep
    trace(mask, getpid());
    trace(mask, 0);
    trace(mask, 1);
    trace(mask, 2);
    trace(mask, 3);
    int pid = fork();//prints fork once
    if (pid == 0) { // child
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
    for (int i = 0; i < 3; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perfs[10];
        int pids[10];
        for (int i = 0; i < 10; i++) {
            pids[i] = wait_stat(&status, &perfs[i]);
        }
        // print all child's pref only after everyone is finished just to keep the printing in order.
        for (int i = 0; i < 10; i++) {
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
        trace(mask, my_pid);
        yield();
        set_priority(2);
        fprintf(2, "Child %d started running\n", my_pid);
        int start = get_ticks();
        int end = 0;
        end = start + 5; // all child will run for 20 ticks
        while (1) { // while true
            if (get_ticks() > end)  // run while loop until reach the correct tick
                break;
        }
    }
}


void sanity_tests2() {
#ifdef CFSD
    int cfsd = 1;
#else
    int cfsd = 0;
#endif
//SchedSanity
//        int pids[NUM_OF_CHILDS];
    for (int i = 0; i < NUM_OF_CHILDS; i++) {
        int pid = -1;
        pid = fork();
        if (pid == 0) { // child
            if (cfsd){
                set_priority(i % 3 + 1);
            }
            if (1){
                small_loop_calculation(); //Calculation only - These processes will perform asimple calculation within a medium sized loop
            }
            if (1)
                large_loop_calculation(); //Calculation only – These processes will perform simple calculation within a very large loop
            if (1)
                small_loop_print(); // Calculation + IO – These processes will perform printing to screen within a medium sized loop
//            if (1)
//                large_loop_print(); // Calculation + IO – These processes will perform printing to screen within a very large loop
            exit(0); // child exit
        }
        int sum_wtime[4];
        int sum_rtime[4];
        int sum_iotime[4];
        for (int i = 0; i < NUM_OF_CHILDS; i++) {
            struct perf perf;
            int status;
            wait_stat(&status, &perf);
            sum_wtime[i % 4] += perf.retime;
            sum_rtime[i % 4] += perf.rutime;
            sum_iotime[i % 4] += perf.stime;
        }
        printf("Calculation Medium -  Wait time: %d,  Run time: %d, IO Time: %d\n\n", sum_wtime[0] / NUM_OF_CHILDS ,
               sum_rtime[0] / NUM_OF_CHILDS , sum_iotime[0] / NUM_OF_CHILDS );
        printf("Calculation Large -  Wait time: %d,  Run time: %d, IO Time: %d\n\n", sum_wtime[1] / NUM_OF_CHILDS,
               sum_rtime[1] / NUM_OF_CHILDS, sum_iotime[1] / NUM_OF_CHILDS);
        printf("Calculation + IO Medium -  Wait time: %d,  Run time: %d, IO Time: %d\n\n",
               sum_wtime[2] / NUM_OF_CHILDS, sum_rtime[2] / NUM_OF_CHILDS, sum_iotime[2] / NUM_OF_CHILDS);
//        printf("Calculation + IO Large -  Wait time: %d,  Run time: %d, IO Time: %d\n\n",
//               sum_wtime[3] / NUM_OF_CHILDS, sum_rtime[3] / NUM_OF_CHILDS, sum_iotime[3] / NUM_OF_CHILDS);
        exit(0);
    }
}

void sanity_tests() {
    int i;
    int n;
    int j = 0;
    int k;
    int sums[3][3];
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            sums[i][j] = 0;
    n = 10; // number of process
    i = n; //unimportant
    int pid;
    for (i = 0; i < 3 * n; i++) {
        j = i % 3;
        pid = fork();
        if (pid == 0) {//child
            j = (getpid() - 4) %
                3; // ensures independence from the first son's pid when gathering the results in the second part of the program
            switch (j) {
                case 0: //CPU‐bound process (CPU):
                    for (k = 0; k < 100; k++) {
                        for (j = 0; j < 1000000; j++) {}
                    }
                    break;
                case 1: //short tasks based CPU‐bound process (S‐CPU):
                    for (k = 0; k < 100; k++) {
                        for (j = 0; j < 1000000; j++) {}
                        yield();
                    }
                    break;
                case 2:// simulate I/O bound process (IO)
                    for (k = 0; k < 100; k++) {
                        sleep(1);
                    }
                    break;
            }
            exit(0); // children exit here
        }
        continue; // father continues to spawn the next child
    }
    for (i = 0; i < 3 * n; i++) {
        struct perf perf;
        int status;
        pid = wait_stat(&status, &perf);
        int res = (pid - 4) % 3; // correlates to j in the dispatching loop
        switch (res) {
            case 0: // CPU bound processes
                printf("CPU-bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid,
                       perf.retime, perf.rutime, perf.stime, perf.retime + perf.rutime + perf.stime);
                sums[0][0] += perf.retime;
                sums[0][1] += perf.rutime;
                sums[0][2] += perf.stime;
                break;
            case 1: // CPU bound processes, short tasks
                printf("CPU-S bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid,
                       perf.retime, perf.rutime, perf.stime, perf.retime + perf.rutime + perf.stime);
                sums[1][0] += perf.retime;
                sums[1][1] += perf.rutime;
                sums[1][2] += perf.stime;
                break;
            case 2: // simulating I/O bound processes
                printf("I/O bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid,
                       perf.retime, perf.rutime, perf.stime, perf.retime + perf.rutime + perf.stime);
                sums[2][0] += perf.retime;
                sums[2][1] += perf.rutime;
                sums[2][2] += perf.stime;
                break;
        }
    }
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            sums[i][j] /= n;
    printf("\n\nCPU bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
           sums[0][0], sums[0][1], sums[0][2], sums[0][0] + sums[0][1] + sums[0][2]);
    printf("CPU-S bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
           sums[1][0], sums[1][1], sums[1][2], sums[1][0] + sums[1][1] + sums[1][2]);
    printf("I/O bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
           sums[2][0], sums[2][1], sums[2][2], sums[2][0] + sums[2][1] + sums[2][2]);
    exit(0);
}

void test_scheduler() {
    int pid = -1;
    int k, nprocess;
    int z, steps;
//    int priority;
    char buffer_src[1024], buffer_dst[1024];
    nprocess = 10;
    steps = 10000000;
    for (k = 0; k < nprocess; k++) {
//        priority = k % 5 + 1;
        // ensure different creation times (proc->ctime)
        // needed for properly testing FCFS scheduling
        sleep(2);
        if (pid != 0)
            pid = fork();
        if (pid < 0) { // parent
            printf("%d failed in fork!\n", getpid());
            exit(0);
        }
    }
    if (pid == 0) { // child
        steps = steps / (20 - getpid());
        // child
//            printf("[pid=%d] created with priority: %d \n", getpid(), priority);
        printf("[pid=%d] created \n", getpid());
//            set_priority(priority);
        for (z = 0; z < steps; z += 1) {
            if (steps % getpid() == 3)
                yield();
            // copy buffers one inside the other and back
            // used for wasting cpu time
            memmove(buffer_dst, buffer_src, 1024);
            memmove(buffer_src, buffer_dst, 1024);
        }
        exit(0);
    }
    int status;
    struct perf perfs[nprocess];
    int pids[nprocess];
    for (k = 0; k < nprocess; k++) {
        pids[k] = wait_stat(&status, &perfs[k]);
        printf("[pid=%d] terminated\n", pids[k]);
    }
    for (k = 0; k < nprocess; k++) {
        printf("child (%d) exited with status %d\n", pids[k], status);
        printf("creation time:    %d\n", perfs[k].ctime);
        printf("termination time: %d\n", perfs[k].ttime);
        printf("running time:     %d\n", perfs[k].rutime);
        printf("runnable time:    %d\n", perfs[k].retime);
        printf("sleeping time:    %d\n", perfs[k].stime);
        printf("average_bursttime:    %d\n", perfs[k].average_bursttime);
    }
    exit(0);
}

void fcfs_test2() {
    int i, pid;
    for (i = 0; i < 10; i++) {
        pid = fork();
        if (pid != 0) // parent
        {
            int cur_pid = getpid();
            printf("%d started\n", cur_pid);
            int j;
            for (j = 0; j < 1000000; j++);
            printf("Round 1 done %d\n", cur_pid);
            for (j = 0; j < 1000000; j++);
            printf("Round 2 done %d\n", cur_pid);
            for (j = 0; j < 1000000; j++);
            printf("Round 3 done %d\n", cur_pid);
            printf("%d ended\n", cur_pid);
            exit(0);
        }
    }
    int status;
    struct perf perfs[10];
    for (i = 0; i < 10; i++) {
        wait_stat(&status, &perfs[i]);
    }
    exit(0);
}


//// this test create 3 child process: first set burst time to: 50000 , second to 100, third to 5000 and all yield
//// expected order of finish: pid 5 -> pid 6 -> pid 4
void srt_test2() {
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
        while (1) { // while true
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
        int end = 0;
        int start = get_ticks();
        if (my_pid == 4) // first child
            end = start + 24;
        else if (my_pid == 5)// second child
            end = start + 3;
        else  // third child
            end = start + 4;
        while (1) { // while true
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
void srt_tets3() {
    fprintf(2, "Hello world!\n");
    struct perf performance;
    int mask = (1 << 1) | (1 << 23) | (1 << 3);
    trace(mask, getpid());
    int cpid = fork();
    if (cpid != 0) {
        int t_pid = wait_stat(0, &performance);
        fprintf(1,
                "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                t_pid, performance.ctime, performance.ttime, performance.stime,
                performance.retime, performance.rutime, performance.average_bursttime);
    } else {
        sleep(10);
        for (int i = 1; i < 15; i++) {
            int c_pid = fork();
            if (c_pid == 0) { // Child
                sleep(i);
                exit(0);
            } else {
                int i = 0;
                while (i < 100000000) {
                    i++;
                }
            }
        }
        sleep(10);
        for (int i = 1; i < 15; i++) {
            int c_pid = fork();
            if (c_pid == 0) {
                int i = 0;
                while (i < 10000000) {
                    i++;
                }
                exit(0);
            } else {
                int t_pid = wait_stat(0, &performance);
                fprintf(1,
                        "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                        t_pid, performance.ctime, performance.ttime, performance.stime, performance.retime,
                        performance.rutime, performance.average_bursttime);
                int i = 0;
                while (i < 10000) {
                    i++;
                }
            }
        }
        int t_pid = wait_stat(0, &performance);
        fprintf(1,
                "terminated pid: %d, ctime: %d, ttime: %d, stime: %d, retime: %d, rutime: %d average_bursttime: %d \n",
                t_pid, performance.ctime, performance.ttime, performance.stime, performance.retime,
                performance.rutime, performance.average_bursttime);
        exit(0);
    }
}

void cfsd_test1() {
    set_priority(1);
    int pid = -1;
    for (int i = 0; i < 5; i++) {
        if (pid != 0) {
            pid = fork();
        }
    }
    if (pid != 0) { // parent
        int status;
        struct perf perfs[5];
        int pids[10];
        for (int i = 0; i < 5; i++) {
            pids[i] = wait_stat(&status, &perfs[i]);
        }
        // print all child's pref only after everyone is finished just to keep the printing in order.
        for (int i = 0; i < 5; i++) {
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
        int end = start + 50;
        if (my_pid == 4) { // first child
            set_priority(1);
        } else if (my_pid == 5) { // second child
            set_priority(5); // super-low
        } else if (my_pid == 6) { // third child
            set_priority(4); // low
        } else if (my_pid == 7) { // 4 child
            set_priority(3); // normal
        } else if (my_pid == 8) { // 5 child
            set_priority(2); // high
        } else if (my_pid == 9) { // 6 child
            set_priority(1); // super-high
            sleep(50);
        }
        while (1) { // while true
            if (get_ticks() > end) // run while loop until reach the correct tick
                break;
        }
    }
}



