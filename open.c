#define _GNU_SOURCE
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>

int open(const char *pathname, int flags){
    syscall(__NR_open, pathname, flags);
    


}
