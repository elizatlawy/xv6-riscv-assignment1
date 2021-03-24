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
        wait(&status);
        fprintf(2, "Child %d finshed with exit status %d\n", pid, status);
    } else { // child
        int my_pid = getpid();
        fprintf(2, "Child %d is running\n", my_pid);
        sbrk(10);
        sleep(10);
    }
    exit(0);
}