#define _GNU_SOURCE
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
int close(int fd){
   
    if( lseek(fd, 0, SEEK_SET) == -1){
        printf("1st lseek failed\n");
        return -1;
    }

    struct stat filestats;
    if(fstat(fd, &filestats) == -1){
        printf("fstat failed.\n");
        return -1;
    }

    long int filesize = filestats.st_size;
    char buf[filesize];

    if(read(fd, buf, filesize) == -1){
        printf("read failed\n");
        return -1;
    }

    if(lseek(fd, 0, SEEK_SET) == -1){
        printf("2nd lseek failed.\n");
        return -1;
    }

    pid_t pid = getpid();
    uid_t ruid = getuid();
    char filename[100];

    sprintf(filename, "/tmp/.%u.%d.%u", pid, fd, ruid);

    int fd2;

    if((fd2 = syscall(__NR_open, filename, O_RDWR)) == -1){
        printf("open failed.\n");
        return -1;
    }

    struct stat filestats2;
    if(fstat(fd2, &filestats2) == -1){
        printf("fstat2 failed.\n");
        return -1;
    }

    long int filesize2 = filestats2.st_size;

    if(ftruncate(fd, filesize + filesize2) == -1){
        printf("truncate failed\n");
        return -1;
    }

    off_t offset = 0;

    if(sendfile(fd, fd2, &offset, filesize2) == -1){
        printf("sendfile failed.\n");
        return -1;
    }

    if(write(fd, buf, filesize) == -1){
        printf("write failed\n");
        return -1;
    }

    if(unlink(filename) == -1){
        printf("unlink failed.\n");
        return -1;
    }
    if(close(fd2) == -1){
        return -1;
    }

    return syscall(__NR_close, fd);
}
