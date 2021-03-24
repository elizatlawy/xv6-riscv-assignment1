#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"



int main(int argc, char** argv) {
    fprintf(1, "starting mask for: SYS_mkdir \n");
    int mask = (1<< 20);
    trace(mask,1);
    
    fprintf(1, "trace end, exit \n");
    exit(0);
}