#define _GNU_SOURCE
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/sendfile.h>

int open(const char *pathname, int flags, ...){

    int fd;
    int check;
    
    //Opens the specified file
    if((fd = syscall(__NR_open, pathname, O_RDWR)) == -1){
        printf("open failed\n");
        return -1;
    }
    
    //Creates filestats struct to extract needed data.
    struct stat filestats;
    if(fstat(fd, &filestats) == -1){
        printf("fstat failed\n");
        return -1;
    }

    long int filesize = filestats.st_size;

    char buffer[filesize];

    //Figure out if there is a virus (deadbeef present?)

    char magic[4] = {0xde, 0xad, 0xbe, 0xef};
    char buf[4];

    do{
        check = read(fd, buf, 4);
        if(check == -1){
           printf("Deadbeef Read Failed\n");
           return -1;
        }
        else if (check == 0){
            printf("Reached end of file. File not infected.\n");
            return fd;
        }

    } while ( memcmp(magic, buf, 4) != 0);

    //Extract virus and place in /tmp/.pid.fd.ruid
    
    off_t virusstop = lseek(fd, 0, SEEK_CUR);
    pid_t pid = getpid();
    uid_t ruid = getuid();
    char filebuf[100];
    sprintf(filebuf, "/tmp/.%u.%d.%u", pid, fd, ruid);    
    int fd2;

    if((fd2 = syscall(__NR_open, filebuf, O_RDWR | O_CREAT)) == -1){
        printf("Virus file open failed.\n");
        return -1;
    }
    off_t zero = 0;
    ssize_t newfile = sendfile(fd2, fd, &zero, virusstop);
    char notvirus[filesize - virusstop];

    if(read(fd, notvirus, filesize - virusstop) == -1){
        printf("reading notvirus into buffer failed.\n");
        return -1;
    }

    if(lseek(fd, 0, SEEK_SET) == -1){
        printf("lseek failed.\n");
        return -1;
    }
    
    if(ftruncate(fd, filesize - virusstop)){
        printf("the truncate failed.\n");
        return -1;
    }

    if((write(fd, notvirus, filesize - virusstop)) == -1){
        printf("write failed \n");
        return -1;
    }
 
    if(close(fd2) == -1){
        printf("close failed\n");
        return -1;
    }
    
    return fd;
}



